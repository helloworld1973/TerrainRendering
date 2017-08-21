#include "Camera.h" 
#include <glm/gtc/matrix_transform.inl> 
#include <GL/freeglut_std.h> 

float Camera::moveSpeed = 3;
float Camera::mouseSensitivity = 0.0005;

static bool* keyStates = new bool[256];
static bool* keySpecialStates = new bool[246];

float Camera::rx = 0;
float Camera::ry = 0;
float Camera::rz = 0;

float Camera::verticalAngle = 0.0f;
float Camera::horizontalAngle = 3.14f;
float Camera::initialFoV = 70.0f;
float Camera::fov = initialFoV;
int Camera::windowWidth = 500;
int Camera::windowHeight = 500;
bool Camera::mouseLock = true;
//bool Camera::wireframe = false;

Camera::Camera()
{
	glutGetWindow();
	//  windowWidth = glutGet(GLenum(GLUT_WINDOW_X)); 
	//  windowHeight = glutGet(GLenum(GLUT_WINDOW_Y)); 
	windowWidth = 1000;
	windowHeight = 1000;
	pos = { 64, 20, -64};
	mouseLock = true;
	wireframe = true;

	proj = glm::perspective(float(glm::radians(20.0)), 1.0f, 10.0f, 1000.0f);
}

void Camera::toggleMouseLock()
{
	mouseLock = !mouseLock;
}

glm::mat4 Camera::apply()
{
	keyOperations();

	if (rx / 360 > 1)
	{
		rx -= 360;
	}
	else if (rx / 360 < -1)
	{
		rx += 360;
	}

	if (ry / 360 > 1)
	{
		ry -= 360;
	}
	else if (ry / 360 < -1)
	{
		ry += 360;
	}

	if (rz / 360 > 1)
	{
		rz -= 360;
	}
	else if (rz / 360 < -1)
	{
		rz += 360;
	}

	glm::mat4 matrix = glm::mat4(1.0);

	glm::mat4 rotationx = rotate(matrix, glm::radians(float(rx)), glm::vec3(1, 0, 0));
	glm::mat4 rotationy = rotate(rotationx, glm::radians(float(ry)), glm::vec3(0, 1, 0));
	glm::mat4 rotationz = rotate(rotationy, glm::radians(float(rz)), glm::vec3(0, 0, 1));

	proj = glm::perspective(float(glm::radians(fov)), 4.0f / 3.0f, 0.1f, 1000.0f);
	view = lookAt(pos, pos + direction(), upVec);

	//reset mouse position 
	if (mouseLock) {
		glutWarpPointer(windowWidth / 2, windowHeight / 2);
	}

	return proj * view * rotationz;
}

glm::mat4 Camera::getView()
{
	return view;
}

glm::vec3 Camera::direction() const
{
	return glm::vec3(cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle));
}

glm::vec3 Camera::right() const
{
	return glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
}

glm::vec3 Camera::up() const
{
	return glm::cross(right(), direction());
}

void Camera::mouseMove(int x, int y)
{
	if (mouseLock) {
		horizontalAngle += mouseSensitivity * float(windowWidth / 2 - x);
		verticalAngle += mouseSensitivity * float(windowHeight / 2 - y);
	}
}

void Camera::mouseDrag(int x, int y)
{

}

void Camera::mouseClick(int button, int state, int x, int y)
{

}

void Camera::mouseScroll(int button, int dir, int x, int y)
{
	fov = initialFoV - 5 * dir * y;
}

void Camera::keyPressed(unsigned char key, int x, int y)
{
	keyStates[key] = true;
	//  if (key == 'c') 
	//  { 
	//    keyStates[key] = !keyStates[key]; 
	//  } 
}

void Camera::keyUp(unsigned char key, int x, int y)
{
	//  if (key != 'c')  
	//  { 
	keyStates[key] = false;
	//  } 
}

void Camera::specialKeyPressed(int key, int x, int y)
{
	keySpecialStates[key] = true;
}

void Camera::specialKeyUp(int key, int x, int y)
{
	keySpecialStates[key] = false;
}

void Camera::keyOperations(void)
{
	if (keySpecialStates[GLUT_KEY_UP])
	{
		moveForward();
	}
	if (keySpecialStates[GLUT_KEY_DOWN])
	{
		moveBackward();
	}
	if (keySpecialStates[GLUT_KEY_LEFT])
	{
		moveLeft();
	}
	if (keySpecialStates[GLUT_KEY_RIGHT])
	{
		moveRight();
	}
	if (keyStates['c'])
	{
		mouseLock = true;
	}
	else
	{
		mouseLock = false;
	}
	if (keyStates['w'])
	{
		wireframe = true;
	}
	else
	{
		wireframe = false;
	}
}

void Camera::setX(float x)
{
	pos.x = x;
}

void Camera::setY(float y)
{
	pos.y = y;
}

void Camera::setZ(float z)
{
	pos.z = z;
}

void Camera::moveY(float dir1, float dir2)
{
}

void Camera::moveZ(float dir1, float dir2)
{
}

void Camera::moveX(float dir1, float dir2)
{
}

void Camera::moveUp()
{
	pos.y += moveSpeed;
}

void Camera::moveDown()
{
	pos.y -= moveSpeed;
}

void Camera::moveForward()
{
	pos += direction() * moveSpeed;
}

void Camera::moveBackward()
{
	pos -= direction() * moveSpeed;
}

void Camera::moveLeft()
{
	pos -= right() * moveSpeed;
}

void Camera::moveRight()
{
	pos += right() * moveSpeed;
}

void Camera::rotateY(int dir)
{
	ry += dir * mouseSensitivity;
}

void Camera::rotateX(int dir)
{
	rx += dir * mouseSensitivity;
}

void Camera::rotatez(int dir)
{
	rz += dir * mouseSensitivity;
}

void Camera::setSpeed(float speed) const
{
	this->moveSpeed = speed;
}

void Camera::addToX(float x)
{
	pos.x += x;
}

void Camera::addToY(float y)
{
	pos.y += y;
}

void Camera::addToZ(float z)
{
	pos.z += z;
}


float Camera::getRy()
{
	return ry;
}

glm::vec3 Camera::getPosition() const
{
	return pos;
}

bool Camera::getWireFrameMode()
{
	return wireframe;
}

