#include "Application.h"

//GLFW includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Learn Open GL Includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

//Imgui Includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

/*
Application* Application::GetInstance()
{
	if (s_pAppInstance == nullptr) {
		s_pAppInstance = new Application();
	}

	return s_pAppInstance;
}
*/

Application::Application()
{
	m_window = nullptr;
	m_fDeltaTime = 0.0f;
	m_fLastFrameTime = 0.0f;
}

Application::~Application()
{
}

bool Application::Initalise()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	//Set Up IMGUI
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	const char* glsl_version = "#version 150";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//Init passed - return true
	return true;

}

void Application::Deinitalise()
{
	//Deinit Imgui
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void Application::Update()
{
	//Start New Imgui Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// per-frame time logic
	// --------------------
	float fCurrentFrameTime = glfwGetTime();
	m_fDeltaTime = fCurrentFrameTime - m_fLastFrameTime;
	m_fLastFrameTime = fCurrentFrameTime;
}

void Application::Render()
{
	// render
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//imgui Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}