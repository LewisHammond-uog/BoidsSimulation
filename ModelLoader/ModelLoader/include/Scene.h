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

	//TODO REMOVE
	int RandomBetweenRange(int fLower, int fUpper);
	float RandomBetweenRange(float fLower, float fUpper);

	GLFWwindow* m_window;
	Camera* m_camera;
	Shader* m_ourShader;
	Model* m_pNanoSuitModel;

	//Mouse Info
	float m_fLastX; //Last X Position of the mouse
	float m_fLastY; //Last Y Position of the mouse
	bool m_bFirstMouse; //If this is the first time that we are geting mouse info

	//Frame Time Info
	float m_fDeltaTime; //Time that the last frame took to render
	float m_fLastFrameTime; //The real time that the last frame started at

	
	static Scene* s_pSceneInstance; //Single instance of this scene;
};


#endif // !__SCENE_H__
