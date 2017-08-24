#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const int MIN_TESS_LEVEL = 1;//by experience
const int MAX_TESS_LEVEL = 5;//by experience
const int VERTEX_BUFFER_STRIDE = 4;//vertex buffer xyzw
const int HEIGHT_SCALE = 12;//by experience
const int GRID_SIZE = 128; //by experience
const int VERTEX_COMPONENTS = 3;//must
const int QUAD_COMPONENTS = 4;//must

//global variables
GLuint vaoID;
GLuint mvpMatrix;
GLuint cameraCoordinate;
GLuint wireFrame;
Camera camera;

float verts[GRID_SIZE * GRID_SIZE * VERTEX_COMPONENTS];
GLushort elems[GRID_SIZE * GRID_SIZE * QUAD_COMPONENTS];
void generateGrid()//generate vertices and indicies
{
	for (int j = 0; j < GRID_SIZE; j++)
	{
		for (int i = 0; i < GRID_SIZE; i++)
		{
			int pos = j * GRID_SIZE + i; 
			int index = pos * VERTEX_COMPONENTS;
			verts[index] = float(j);
			verts[index + 1] = 0.0f;
			verts[index + 2] = float(-i);//every vertex has X Y Z 3-coordinates

			if (j != (GRID_SIZE - 1) && i != (GRID_SIZE - 1))
			{
				int epos = pos * QUAD_COMPONENTS; 
				int npos = pos + GRID_SIZE;
				elems[epos] = pos;
				elems[epos + 1] = npos;
				elems[epos + 2] = npos + 1;
				elems[epos + 3] = pos + 1;//every patch is a quad(4 points-4index)
			}
		}
	}
}

void loadTGA(string filename)
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

void loadTextures()
{
	GLuint heightMap;
	glGenTextures(1, &heightMap);   //Generate 1 texture ID 
	glActiveTexture(GL_TEXTURE0);  //Texture unit 0 
	glBindTexture(GL_TEXTURE_2D, heightMap);
	loadTGA("HeightMap.tga");  // Load heightMap 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//Mipmap,Texture Filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint texWater;
	glGenTextures(1, &texWater);
	glActiveTexture(GL_TEXTURE1);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texWater);
	loadTGA("water.tga"); // water
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint texGrass;
	glGenTextures(1, &texGrass);
	glActiveTexture(GL_TEXTURE2);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texGrass);
	loadTGA("grass.tga"); // grass
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint texRock;
	glGenTextures(1, &texRock);
	glActiveTexture(GL_TEXTURE3);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texRock);
	loadTGA("rock.tga"); // rock
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLuint texSnow;
	glGenTextures(1, &texSnow);
	glActiveTexture(GL_TEXTURE4);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texSnow);
	loadTGA("snow.tga"); // snow
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

GLuint loadShader(GLenum shaderType, string filename)
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

void initialise()
{
//1.generate vertices and indicies, then put them into the buffer
	generateGrid();//generate vertices and indicies
	glGenVertexArrays(1, &vaoID);//VAO start
	glBindVertexArray(vaoID);
	GLuint vboID[2];
	glGenVertexArrays(2, vboID);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);  // Vertex position (location=0)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);//indicies
	glBindVertexArray(0);//VAO end


//2.generate shaders,then attach them to the program 
	GLuint shaderVert = loadShader(GL_VERTEX_SHADER, "TR.vert");
	GLuint shaderTcs = loadShader(GL_TESS_CONTROL_SHADER, "TR.tcs");
	GLuint shaderTes = loadShader(GL_TESS_EVALUATION_SHADER, "TR.tes");
	GLuint shaderGeom = loadShader(GL_GEOMETRY_SHADER, "TR.geom");
	GLuint shaderFrag = loadShader(GL_FRAGMENT_SHADER, "TR.frag");

	GLuint program = glCreateProgram();
	glAttachShader(program, shaderVert);
	glAttachShader(program, shaderTcs);
	glAttachShader(program, shaderTes);
	glAttachShader(program, shaderGeom);
	glAttachShader(program, shaderFrag);
	glLinkProgram(program);//introduce shaders
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program); // check shader status  if OK, then go on


//3.get the uniform variables of all the shaders(glGetUniformLocation),in order to set value in it(glUniform)
	//some uniform variables can set value right now,because they are not change during rendering process
	mvpMatrix = glGetUniformLocation(program, "mvpMatrix");
	//uniform variable in vertex shader
	GLuint minTessLevel = glGetUniformLocation(program, "minTessLevel");
	glUniform1i(minTessLevel, MIN_TESS_LEVEL);
	GLuint maxTessLevel = glGetUniformLocation(program, "maxTessLevel");
	glUniform1i(maxTessLevel, MAX_TESS_LEVEL);
	GLuint vertexBufferStride = glGetUniformLocation(program, "vertexBufferStride");
	glUniform1i(vertexBufferStride, VERTEX_BUFFER_STRIDE);// Tessilation levels
	GLuint gridSize = glGetUniformLocation(program, "gridSize");
	glUniform1f(gridSize, GRID_SIZE);
	loadTextures();// load textures for heightMap,water,grass,rock,snow
	GLuint textureLocation = glGetUniformLocation(program, "heightMap");
	glUniform1i(textureLocation, 0);//assign a value 0 to the variable textureLocation to specify that it should use the texture from unit 0.
	cameraCoordinate = glGetUniformLocation(program, "cameraCoordinate");
    //uniform variable in TCS shader
	GLuint heightScale = glGetUniformLocation(program, "heightScale");
	glUniform1i(heightScale, HEIGHT_SCALE);// Height modifier
	//uniform variable in TES shader
	glm::vec3 lightCoordinate = glm::vec3(0 , 60.0, 0 );
	GLuint lightCoord = glGetUniformLocation(program, "lightCoord");
	glUniform3fv(lightCoord, 1, &lightCoordinate[0]);// Lighting Coordinate
	//uniform variable in geometry shader
	wireFrame = glGetUniformLocation(program, "wireframe");
	textureLocation = glGetUniformLocation(program, "water");
	glUniform1i(textureLocation, 1);
	textureLocation = glGetUniformLocation(program, "grass");
	glUniform1i(textureLocation, 2);
	textureLocation = glGetUniformLocation(program, "rock");
	glUniform1i(textureLocation, 3);
	textureLocation = glGetUniformLocation(program, "snow");
	glUniform1i(textureLocation, 4);
	//uniform variable in fragment shader


//4.change the camera's position with input equipments
	camera = Camera();
	glutKeyboardFunc(Camera::keyPress);
	glutKeyboardUpFunc(Camera::keyLoosen);
	glutPassiveMotionFunc(Camera::mouseMove);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


void update(int value)
{
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}


void display()
{
	//set mvp matrix
	glUniformMatrix4fv(mvpMatrix, 1, GL_FALSE, &camera.updateMVPMatrix()[0][0]);

	//set camera position
	glm::vec4 cameraPos = glm::vec4(camera.getCameraPosition(), 0);
	glUniform4fv(cameraCoordinate, 1, &cameraPos[0]);

	//set wireframe mode
	int wireFrameFlag = 0;
	if (camera.getWireFrameMode())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireFrameFlag = 1;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireFrameFlag = 0;
	}
	glUniform1i(wireFrame, wireFrameFlag);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);//invoke VAO
	glDrawElements(GL_PATCHES, GRID_SIZE * GRID_SIZE * 4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Terrain Rendering Project");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	if (glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(50, update, 0);
	glutMainLoop();
}