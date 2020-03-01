#include "Scene.h"

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

//Project Includes
#include "Entity.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "BrainComponent.h"
#include "DebugUI.h"

//Screen Size Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

const unsigned int BOID_COUINT = 25;

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
	m_fLastX = SCR_WIDTH / 2.0f;
	m_fLastY = SCR_HEIGHT / 2.0f;
	m_bFirstMouse = true;
	m_camera = nullptr;
	m_window = nullptr;
	m_fDeltaTime = 0.0f;
	m_fLastFrameTime = 0.0f;
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
	m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
	glfwSetCursorPosCallback(m_window, mouse_callback);
	glfwSetScrollCallback(m_window, scroll_callback);

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

	// build and compile shaders
	// -------------------------
	m_ourShader = new Shader("shaders/model_loading.vs", "shaders/model_loading.fs");

	// load models
	// -----------
	m_pNanoSuitModel = new Model("models/nanosuit/nanosuit.obj");

	//Init Camera
	m_camera = new Camera(glm::vec3(0.0f, 1.2f, 4.0f));

	//Seed RNG
	srand(time(nullptr));

	//Create entities
	for (int i = 0; i < BOID_COUINT; i++) {
		Entity* pEntity = new Entity();

		//Transform Component
		TransformComponent* pTransform = new TransformComponent(pEntity);
		pTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(RandomBetweenRange(-5.0f, 5.0f), 
																			RandomBetweenRange(-5.0f, 5.0f),
																			RandomBetweenRange(-5.0f, 5.0f)));
		pEntity->AddComponent(pTransform);

		//Model Component
		ModelComponent* pModel = new ModelComponent(pEntity);
		pModel->SetModel(m_pNanoSuitModel);
		pModel->SetScale(0.02f);
		pEntity->AddComponent(pModel);

		//Brain Component
		BrainComponent* pBrain = new BrainComponent(pEntity);
		pEntity->AddComponent(pBrain);
		
	}

	return true;
}

bool Scene::Update() {

	//Start New Imgui Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// per-frame time logic
	// --------------------
	float fCurrentFrameTime = glfwGetTime();
	m_fDeltaTime = fCurrentFrameTime - m_fLastFrameTime;
	m_fLastFrameTime = fCurrentFrameTime;

	// input
	// -----
	m_camera->processInput(m_window, m_fDeltaTime);

	DebugUI::GetInstance()->Update();

	//Update Boids
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter) 
	{
		Entity* pEntity = xIter->second;
		if (pEntity) {
			pEntity->Update(m_fDeltaTime);
		}
	}

	//return if we should keep running
	return !glfwWindowShouldClose(m_window);
}

void Scene::Render() {

	// render
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_ourShader || !m_pNanoSuitModel) {
		return;
	}

	// don't forget to enable shader before setting uniforms
	m_ourShader->use();

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = m_camera->GetViewMatrix();
	m_ourShader->setMat4("projection", projection);
	m_ourShader->setMat4("view", view);

	//Draw Boids
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter)
	{
		Entity* pEntity = xIter->second;
		if (pEntity) {
			pEntity->Draw(m_ourShader);
		}
	};

	//imgui Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(m_window);
	glfwPollEvents();

}


void Scene::DeInitlise() {

	delete m_camera;
	delete m_pNanoSuitModel;
	delete m_ourShader;

	//Delete all of the entities that exist in the scene
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	//Make a duplicate of the entity map so that we don't cause problems
	//when itterating through it
	std::map<const unsigned int, Entity*> existingEntityMap = Entity::GetEntityMap();
	//Loop through all of the entities that exist and delete the
	for (xIter = existingEntityMap.begin(); xIter != existingEntityMap.end(); ++xIter)
	{
		Entity* pEntity = xIter->second;
		if (pEntity) {
			delete pEntity;
		}
	}

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

	if (pScene->m_bFirstMouse)
	{
		pScene->m_fLastX = xpos;
		pScene->m_fLastY = ypos;
		pScene->m_bFirstMouse = false;
	}

	float xoffset = xpos - pScene->m_fLastX;
	float yoffset = pScene->m_fLastY - ypos; // reversed since y-coordinates go from bottom to top

	pScene->m_fLastX = xpos;
	pScene->m_fLastY = ypos;

	if (pScene->m_camera) {
		pScene->m_camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Scene::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Scene* pScene = Scene::GetInstance();
	if (!pScene && !pScene->m_camera) {
		return;
	}

	pScene->m_camera->ProcessMouseScroll(yoffset);
}

int Scene::RandomBetweenRange(int fLower, int fUpper)
{
	return (rand() % (glm::abs(fLower - fUpper)) + fLower);
}

float Scene::RandomBetweenRange(float fLower, float fUpper)
{
	return fLower + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (fUpper - fLower)));
}