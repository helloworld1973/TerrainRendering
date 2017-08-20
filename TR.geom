#version 400

layout(triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform vec3 lightPos;
uniform mat4 mvpMatrix;
uniform int heightScale;

out float diffuse;
out vec2 texCoord;
out vec4 texWeight;

void main()
{
	int i;
	vec3 ab = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 ac = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = normalize(cross(ab, ac));
	
	for (i = 0; i < gl_in.length(); i++)
	{
		vec4 posn = gl_in[i].gl_Position;
		vec3 lightVec = lightPos - posn.xyz;
		diffuse = max(dot(normal, normalize(lightVec)), 0);
		texCoord = posn.xz;
		
		if (posn.y < (2 * heightScale / 12)) {
			texWeight = vec4(1, 0, 0, 0);
		}
		else if (posn.y < heightScale / 2) {
			texWeight = vec4(0, 1, 0, 0);
		}
		else if (posn.y < (5 * heightScale / 8))
		{
			float period = (5 * heightScale / 8) - (heightScale / 2);
			float point = posn.y - (heightScale / 2);
			float percentageRock = point / period;
			float percentageGrass = 1 - percentageRock;
			texWeight = vec4(0, percentageGrass, percentageRock, 0);
		}
		else if (posn.y < (3 * heightScale / 4)) {
			texWeight = vec4(0, 0, 1, 0);
		}
		else if (posn.y < (7 * heightScale / 8))
		{
			float period = (7 * heightScale / 8) - (3 * heightScale / 4);
			float point = posn.y - (3 * heightScale / 4);
			float percentageSnow = point / period;
			float percentageRock = 1 - percentageSnow;
			texWeight = vec4(0, 0, percentageRock, percentageSnow);
		}
		else {
			texWeight = vec4(0, 0, 0, 1);
		}

		gl_Position = mvpMatrix * posn;

		EmitVertex();
	}
	EndPrimitive();

}
