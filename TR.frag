#version 400

uniform sampler2D water;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform int wireframe;

in float diffLight;
in float specularLight;
in vec2 textureCoord;
in vec4 textureWeight;
out vec4 FragColor;

void main() 
{
	vec4 texWater = texture(water, textureCoord);
	vec4 texGrass = texture(grass, textureCoord);
	vec4 texRock = texture(rock, textureCoord);
	vec4 texSnow = texture(snow, textureCoord);

	vec4 diffuseVec = vec4(diffLight, diffLight, diffLight, 1);

	if (wireframe == 1) 
	{
		FragColor = vec4(1, 1, 1, 1) * diffuseVec;
		return;//wireframe model 
	}
	
	FragColor = (texWater * textureWeight.x*(vec4(1, 0, 0, 1)*specularLight)) //specular Light
	          + (texWater * textureWeight.x+texGrass * textureWeight.y + texRock * textureWeight.z + texSnow * textureWeight.w) * diffLight*vec4(1, 1, 0.92, 0.6);//diffuse Light
            //texture model
}