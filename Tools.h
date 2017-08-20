#include <GL/glew.h> 
#include <fstream> 

using namespace std;

class Tools
{
public:
	static GLuint loadShader(GLenum shaderType, string filename);
	static void loadTGA(string filename);
};