#include "Tools.h"
#include <GL/glew.h> 
#include <iostream> 
#include <fstream> 
#include <sstream> 
using namespace std;

GLuint Tools::loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if (!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		std::cerr << "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

void Tools::loadTGA(string filename)
{
	char id, cmap, imgtype, bpp, c_garb;
	char* imageData, temp;
	short int s_garb, wid, hgt;
	int nbytes, size, indx;
	ifstream file(filename.c_str(), ios::in | ios::binary);
	if (!file)
	{
		cout << "*** Error opening image file: " << filename.c_str() << endl;
		exit(1);
	}
	file.read(&id, 1);
	file.read(&cmap, 1);
	file.read(&imgtype, 1);
	if (imgtype != 2 && imgtype != 3)   //2= colour (uncompressed),  3 = greyscale (uncompressed) 
	{
		cout << "*** Incompatible image type: " << (int)imgtype << endl;
		exit(1);
	}
	//Color map specification 
	file.read((char*)&s_garb, 2);
	file.read((char*)&s_garb, 2);
	file.read(&c_garb, 1);
	//Image specification 
	file.read((char*)&s_garb, 2);  //x origin 
	file.read((char*)&s_garb, 2);  //y origin 
	file.read((char*)&wid, 2);     //image width                     
	file.read((char*)&hgt, 2);     //image height 
	file.read(&bpp, 1);     //bits per pixel 
	file.read(&c_garb, 1);  //img descriptor 
	nbytes = bpp / 8;           //No. of bytes per pixels 
	size = wid * hgt * nbytes;  //Total number of bytes to be read 
	imageData = new char[size];
	file.read(imageData, size);
	//cout << ">>>" << nbytes << " " << wid << " " << hgt << endl; 
	if (nbytes > 2)   //swap R and B 
	{
		for (int i = 0; i < wid*hgt; i++)
		{
			indx = i*nbytes;
			temp = imageData[indx];
			imageData[indx] = imageData[indx + 2];
			imageData[indx + 2] = temp;
		}
	}

	switch (nbytes)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, 1, wid, hgt, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, 3, wid, hgt, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, 4, wid, hgt, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		break;
	}

	delete imageData;
}