#ifndef __SCENE_H__
#define __SCENE_H__

//Foward Declares
struct GLFWwindow;
class Camera;
class Shader;
class Model;

class Scene
{
public:
	static Scene* GetInstance();

	bool Initalise();
	bool Update();
	void Render();
	void DeInitlise();

private:
	Scene();
	Scene(const Scene&); //Overload Copy Constructor - no implementation
	Scene& operator=(const Scene&); //Overload Equals Operator - no implementation

	//OpenGL Callback functions
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* window;
	Camera* camera;
	Shader* ourShader;
	Model* ourModel;

	//TO DO CODING STANDARS
	float lastX;
	float lastY;
	bool firstMouse;
	float deltaTime;
	float lastFrame;


	static Scene* s_pSceneInstance; //Single instance of this scene;
};


#endif // !__SCENE_H__
