#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "Singleton.h"

//Forward Declares
struct GLFWwindow;

/// <summary>
/// Application Class which is a parent of scene
/// to deal the with initalisation of OpenGL, GLFW
/// and other 3rd party initalisations
/// </summary>
class Application
{
protected:

	Application();
	virtual ~Application() = default;

	//Iniitalise/Deinitialize the application
	bool Initialize();
	void Deinitialize();

	//Update Application
	virtual bool Update();
	virtual void Render();
	

	//Window that the application is rendering to
	GLFWwindow* m_window;

	//Frame Time Info
	float m_fDeltaTime; //Time that the last frame took to render
	float m_fLastFrameTime; //The real time that the last frame started at

	//Screen Size Settings
	const unsigned int SCR_WIDTH = 1280;
	const unsigned int SCR_HEIGHT = 800;

private:

	//OpenGL Callback functions
	static void framebuffer_size_callback(GLFWwindow* a_window, int a_width, int a_height);


};

#endif // !__APPLICATION_H__

