#include <glm/glm.hpp> 

class Camera
{
public:
	 static float moveSpeed;
	 static float mouseSense;
	 bool wireframe;
	 static float fov;
	 static int windowWidth;
	 static int windowHeight;
	 static bool mouseLock;
	 static float theta;
	 static float alpha;
	 
	 Camera();
	 glm::mat4 initApply();
	 glm::mat4 apply();
	 glm::vec3 getCameraPosition();	
	 bool getWireFrameMode();
	
	 static void mouseMove(int x, int y);
	 static void keyPress(unsigned char key, int x, int y);
	 static void keyLoosen(unsigned char key, int x, int y);
	
	 void moveForward();
	 void moveBackward();
	 void moveLeft();
	 void moveRight();
	 glm::vec3 cameraFront();
	 glm::vec3 right();

	 glm::vec3 cameraPosition;
	 glm::vec3 initLookAt={128/2,0,-128/2};
	 glm::mat4 projMatrix;
	 glm::mat4 viewMatrix;
	 glm::vec3 cameraUp = { 0, 1, 0 };
};
