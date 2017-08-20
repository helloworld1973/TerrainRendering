#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Tools.h"
#include "Camera.h"
using namespace std;

GLuint vaoID;
GLuint theProgram;
GLuint matrixLoc;
GLuint cameraLoc;
GLuint lightloc;
GLuint wireFrameLoc;
GLuint heightMap;
GLuint texWater;
GLuint texGrass;
GLuint texRock;
GLuint texSnow;
Camera camera;
int defaultTessLevel;
int maxTessLevel;
int slopePointLookAhead;
glm::vec3 light;


const int GRID_SIZE = 128; // 512;
const int HEIGHT_SCALE = 10;
const int VERTEX_COMPONENTS = 3;
const int QUAD_COMPONENTS = 4;
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


void loadTextures()
{
	glGenTextures(1, &heightMap);   //Generate 1 texture ID 
	glActiveTexture(GL_TEXTURE0);  //Texture unit 0 
	glBindTexture(GL_TEXTURE_2D, heightMap);
	Tools::loadTGA("HeightMap.tga");  // Load heightMap 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texWater);
	glActiveTexture(GL_TEXTURE1);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texWater);
	Tools::loadTGA("water.tga"); // water
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texGrass);
	glActiveTexture(GL_TEXTURE2);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texGrass);
	Tools::loadTGA("grass.tga"); // grass
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texRock);
	glActiveTexture(GL_TEXTURE3);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texRock);
	Tools::loadTGA("rock.tga"); // rock
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &texSnow);
	glActiveTexture(GL_TEXTURE4);  //Texture unit 1 
	glBindTexture(GL_TEXTURE_2D, texSnow);
	Tools::loadTGA("snow.tga"); // snow
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
	GLuint shaderVert = Tools::loadShader(GL_VERTEX_SHADER, "TR.vert");
	GLuint shaderTcs = Tools::loadShader(GL_TESS_CONTROL_SHADER, "TR.tcs");
	GLuint shaderTes = Tools::loadShader(GL_TESS_EVALUATION_SHADER, "TR.tes");
	GLuint shaderGeom = Tools::loadShader(GL_GEOMETRY_SHADER, "TR.geom");
	GLuint shaderFrag = Tools::loadShader(GL_FRAGMENT_SHADER, "TR.frag");

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
	matrixLoc = glGetUniformLocation(program, "mvpMatrix");
	//uniform variable in vertex shader
	defaultTessLevel = 1;
	maxTessLevel = 10;
	slopePointLookAhead = 4;
	GLuint dtl = glGetUniformLocation(program, "defaultTessLevel");
	glUniform1i(dtl, defaultTessLevel);
	GLuint mtl = glGetUniformLocation(program, "maxTessLevel");
	glUniform1i(mtl, maxTessLevel);
	GLuint slopePointLoc = glGetUniformLocation(program, "slopeDistance");
	glUniform1i(slopePointLoc, slopePointLookAhead);// Tessilation levels
	GLuint gridMax = glGetUniformLocation(program, "gridSizeMax");
	glUniform1f(gridMax, GRID_SIZE);
	loadTextures();// load textures for heightMap,water,grass,rock,snow
	GLuint texLoc = glGetUniformLocation(program, "heightMap");
	glUniform1i(texLoc, 0);//assign a value 0 to the variable texLoc to specify that it should use the texture from unit 0.
	cameraLoc = glGetUniformLocation(program, "cameraVector");
    //uniform variable in TCS shader
	GLuint heightLoc = glGetUniformLocation(program, "heightScale");
	glUniform1i(heightLoc, HEIGHT_SCALE);// Height modifier
	//uniform variable in TES shader
	light = glm::vec3(GRID_SIZE / 2, 1000.0, GRID_SIZE / 2);
	lightloc = glGetUniformLocation(program, "lightPos");
	glUniform3fv(lightloc, 1, &light[0]);// Lighting
	//uniform variable in geometry shader
	wireFrameLoc = glGetUniformLocation(program, "wireframe");
	texLoc = glGetUniformLocation(program, "water");
	glUniform1i(texLoc, 1);
	texLoc = glGetUniformLocation(program, "grass");
	glUniform1i(texLoc, 2);
	texLoc = glGetUniformLocation(program, "rock");
	glUniform1i(texLoc, 3);
	texLoc = glGetUniformLocation(program, "snow");
	glUniform1i(texLoc, 4);
	//uniform variable in fragment shader


//4.change the camera's position with input equipments
	camera = Camera();
	glutKeyboardFunc(Camera::keyPressed);
	glutKeyboardUpFunc(Camera::keyUp);
	glutSpecialFunc(Camera::specialKeyPressed);
	glutSpecialUpFunc(Camera::specialKeyUp);
	glutMotionFunc(Camera::mouseDrag);
	glutPassiveMotionFunc(Camera::mouseMove);
	glutMouseFunc(Camera::mouseClick);
	glutMouseWheelFunc(Camera::mouseScroll);// Setup Camera 


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
	glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &camera.apply()[0][0]);

	//set camera position
	glm::vec4 cameraPos = glm::vec4(camera.getPosition(), 0);
	glUniform4fv(cameraLoc, 1, &cameraPos[0]);

	//set wireframe mode
	int wireframeInt = 0;
	if (camera.getWireFrameMode())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframeInt = 1;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframeInt = 0;
	}
	glUniform1i(wireFrameLoc, wireframeInt);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
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