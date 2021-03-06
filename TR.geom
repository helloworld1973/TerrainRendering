#version 400

layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform vec3 lightCoord;
uniform mat4 mvpMatrix;
uniform int heightScale;
uniform vec4 cameraCoordinate;

out float diffLight;
out float specularLight;
out vec2 textureCoord;
out vec4 textureWeight;

void main()
{
	vec3 vec0to1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 vec0to2 = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	vec3 vecNormal = normalize(cross(vec0to1, vec0to2));
	//calculate the normal vector(unit) of the triangle
	
	float specularLight=1.0;//specular Light
	
	for (int i = 0; i < gl_in.length(); i++)
	{
		vec4 position = gl_in[i].gl_Position;
		vec3 lightVec = lightCoord - position.xyz;//light vector
		diffLight = max(dot(vecNormal, normalize(lightVec)), 0);// cos# between light vector to normal vector
		textureCoord = position.xz;
		
		//different heightValue has different geologies
		if (position.y == 2.5) 
		{
			textureWeight = vec4(1, 0, 0, 0);

			//water regions has specular Light
			float specularStrength = 1;
			vec3 viewVec=normalize(cameraCoordinate.xyz-position.xyz);
			vec3 reflectVec = reflect(lightVec, vecNormal);
			float spec = pow(max(dot(viewVec, reflectVec), 0.0), 2);
            specularLight = specularStrength * spec;
		}
		else if (position.y < 5.5) 
		{
			textureWeight = vec4(0, 1, 0, 0);
		}
		else if (position.y < 6.0)
		{
			float period = 0.5;
			float point = position.y - 5.5;
			float percentageRock = point / period;
			float percentageGrass = 1 - percentageRock;
			textureWeight = vec4(0, percentageGrass, percentageRock, 0);
		}
		else if (position.y < 10.0) 
		{
			textureWeight = vec4(0, 0, 1, 0);
		}
		else if (position.y < 10.5)
		{
			float period = 0.5;
			float point = position.y - 10.0;
			float percentageSnow = point / period;
			float percentageRock = 1 - percentageSnow;
			textureWeight = vec4(0, 0, percentageRock, percentageSnow);
		}
		else 
		{
			textureWeight = vec4(0, 0, 0, 1);
		}

		gl_Position = mvpMatrix * position;

		EmitVertex();
	}
	EndPrimitive();

}
