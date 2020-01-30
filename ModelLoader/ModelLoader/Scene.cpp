#include "Scene.h"

//GLFW includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Learn Open GL Includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

//Screen Size Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

Scene* Scene::s_pSceneInstance = nullptr;

///Get singleton instance of scene
Scene* Scene::GetInstance() {

	//Create an instance if we don't have one
	if (s_pSceneInstance == nullptr) {
		s_pSceneInstance = new Scene();
	}

	return s_pSceneInstance;
}


Scene::Scene() {
	//Init Values
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;
	firstMouse = true;
	camera = nullptr;
	window = nullptr;
	deltaTime = 0.0f;
	lastFrame = 0.0f;
}

bool Scene::Initalise() {

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	ourShader = new Shader("shaders/model_loading.vs", "shaders/model_loading.fs");

	// load models
	// -----------
	ourModel = new Model("models/nanosuit/nanosuit.obj");

	//Init Camera
	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	return true;
}

bool Scene::Update() {

	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// input
	// -----
	camera->processInput(window, deltaTime);

	//return if we should keep running
	return !glfwWindowShouldClose(window);
}

void Scene::Render() {

	// render
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!ourShader || !ourModel) {
		return;
	}

	// don't forget to enable shader before setting uniforms
	ourShader->use();

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	ourShader->setMat4("model", model);
	ourModel->Draw(*ourShader);


	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);
	glfwPollEvents();

}


void Scene::DeInitlise() {

	delete camera;
	delete ourModel;
	delete ourShader;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Scene::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	Scene* pScene = Scene::GetInstance();
	if (!pScene) {
		return;
	}

	if (pScene->firstMouse)
	{
		pScene->lastX = xpos;
		pScene->lastY = ypos;
		pScene->firstMouse = false;
	}

	float xoffset = xpos - pScene->lastX;
	float yoffset = pScene->lastY - ypos; // reversed since y-coordinates go from bottom to top

	pScene->lastX = xpos;
	pScene->lastY = ypos;

	if (pScene->camera) {
		pScene->camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Scene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Scene* pScene = Scene::GetInstance();
	if (!pScene && !pScene->camera) {
		return;
	}

	pScene->camera->ProcessMouseScroll(yoffset);
}