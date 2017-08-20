#include <glm/glm.hpp> 

class Camera
{
public:
	static float moveSpeed;
	static float mouseSensitivity;
	Camera();
	glm::mat4 getView();
	static void toggleMouseLock();
	void keyOperations();
	glm::mat4 apply();
	static void mouseMove(int x, int y);
	static void mouseDrag(int x, int y);
	static void mouseClick(int button, int state, int x, int y);
	static void mouseScroll(int button, int dir, int x, int y);
	static void keyPressed(unsigned char key, int x, int y);
	static void keyUp(unsigned char key, int x, int y);
	static void specialKeyPressed(int key, int x, int y);
	static void specialKeyUp(int key, int x, int y);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void moveY(float dir1, float dir2);
	void moveZ(float dir1, float dir2);
	void moveX(float dir1, float dir2);
	void moveUp();
	void moveDown();
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	static void rotateY(int dir);
	static void rotateX(int dir);
	static void rotatez(int dir);
	void setSpeed(float speed) const;
	void addToX(float x);
	void addToY(float y);
	void addToZ(float z);
	static float getRy();
	glm::vec3 getPosition() const;
	bool getWireFrameMode();
private:
	glm::vec3 pos;
	glm::mat4 projView;
	glm::mat4 prodMatrix;
	glm::mat4 proj;
	glm::mat4 view;
	glm::vec3 upVec = { 0, 1, 0 };
	glm::vec3 zeroVec = { 0, 0,0 };
	bool wireframe = true;
	static float fov;
	static int windowWidth;
	static int windowHeight;
	static bool mouseLock;
	static float rx;
	static float ry;
	static float rz;
	static float verticalAngle;
	static float horizontalAngle;
	static float initialFoV;
	glm::vec3 direction() const;
	glm::vec3 right() const;
	glm::vec3 up() const;
};
