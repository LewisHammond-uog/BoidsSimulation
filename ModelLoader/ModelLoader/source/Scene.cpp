#include "Scene.h"

//GLFW includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Gizmo Includes
#include <Gizmos.h>

//Learn Open GL Includes
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

//Imgui Includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//C++ Includes
#include <iostream>

//Project Includes
#include "Entity.h"
#include "DebugUI.h"

//Components
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "BrainComponent.h"
#include "ColliderComponent.h"
#include "BoxPrimitiveComponent.h"
#include "MathsUtils.h"
#include "SpherePrimitiveComponent.h"


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


Scene::Scene() : Application() {
	//Init camera
	m_camera = nullptr;
}

bool Scene::Initalise(){

	//Call Application Init Function
	if (!Application::Initalise()) {
		//return false if application did not
		//initalise properly
		return false;
	}
	Gizmos::create();
	
	//Init Callback functions for mouse move/scroll
	glfwSetCursorPosCallback(m_window, mouse_callback);
	glfwSetScrollCallback(m_window, scroll_callback);

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

	//Create a collision world - this is the physics simulation that all of our physics will
	//occour in
	m_pSceneCollisionWorld = new rp3d::CollisionWorld;


	//Create our world bounds
	GenerateBoundsVolume(10.f);
	
	//Create entities
	for (int i = 0; i < BOID_COUINT; i++) {
		Entity* pEntity = new Entity();

		//Transform Component
		TransformComponent* pTransform = new TransformComponent(pEntity);
		pTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(	MathsUtils::RandomRange(-5.0f, 5.0f), 
																				MathsUtils::RandomRange(-5.0f, 5.0f),
																				MathsUtils::RandomRange(-5.0f, 5.0f)));
		pEntity->AddComponent(pTransform);

		//Model Component
		ModelComponent* pModel = new ModelComponent(pEntity);
		pModel->SetModel(m_pNanoSuitModel);
		pModel->SetScale(0.02f);
		pEntity->AddComponent(pModel);

		//Brain Component
		BrainComponent* pBrain = new BrainComponent(pEntity);
		pEntity->AddComponent(pBrain);

		//Collider Component
		ColliderComponent* pCollider = new ColliderComponent(pEntity, m_pSceneCollisionWorld);
		pCollider->AddSphereCollider(0.25f, glm::vec3(0.0f));
		pEntity->AddComponent(pCollider);
	}

	return true;
}

bool Scene::Update() {

	//Update the application values
	Application::Update();

	//Clear the Gizmos from last frame
	Gizmos::clear();
	
	// input
	// -----
	m_camera->processInput(m_window, m_fDeltaTime);

	//Update the Debug UI
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
	
	//Call Render on Application
	Application::Render();
	
	if (!m_ourShader || !m_pNanoSuitModel) {
		return;
	}

	// don't forget to enable shader before setting uniforms
	m_ourShader->use();

	// view/projection transformations
	const glm::mat4 projection = glm::perspective(glm::radians(m_camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	const glm::mat4 view = m_camera->GetViewMatrix();
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
	}

	//Draw Gizmos
	Gizmos::draw(view, projection);

	//imgui Render
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}


void Scene::DeInitlise() {

	//Deinitalise Application
	Application::Deinitalise();

	if (m_camera != nullptr) {
		delete m_camera;
	}
	if (m_pNanoSuitModel != nullptr) {
		delete m_pNanoSuitModel;
	}
	if (m_ourShader != nullptr) {
		delete m_ourShader;
	}

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

	//Destory Collision World
	if (m_pSceneCollisionWorld != nullptr) {

		delete m_pSceneCollisionWorld;
	}

	//Destory Gizmos
	Gizmos::destroy();

}

/// <summary>
/// Get the collision world that the scene is using
/// </summary>
/// <returns></returns>
rp3d::CollisionWorld* Scene::GetCollisionWorld() const
{
	return m_pSceneCollisionWorld;
}

/// <summary>
/// Generate a square bounds volume of the given size
/// </summary>
/// <param name="a_fBoundsSize">Size of the bounds to generate</param>
void Scene::GenerateBoundsVolume(const float a_fBoundsSize) const
{
	constexpr float fWallThickness = 0.1f;
	constexpr int iWallCount = 6;

	//todo - there has to be a better way to do this
	//Array of the wall sizes and positions that we need
	glm::vec3 aV3wallSizes[3] = {	glm::vec3(a_fBoundsSize,a_fBoundsSize,fWallThickness),
									glm::vec3(a_fBoundsSize,fWallThickness,a_fBoundsSize),
									glm::vec3(fWallThickness,a_fBoundsSize,a_fBoundsSize) };
	glm::vec3 aV3wallPositions[iWallCount] = {	glm::vec3(0.f,0.f,a_fBoundsSize), glm::vec3(0.f,0.f,-a_fBoundsSize),
										glm::vec3(0.f,a_fBoundsSize,0.0f), glm::vec3(0.f,-a_fBoundsSize,0.f),
										glm::vec3(a_fBoundsSize,0.0f,0.0f), glm::vec3(-a_fBoundsSize,0.0f,0.f), };

	
	for(int i = 0; i < iWallCount; ++i)
	{
		//Get the size of the current wall
		const glm::vec3 currentWallSize = aV3wallSizes[i / 2];
		//Get the position of the current Wall
		const glm::vec3 currentWallPosition = aV3wallPositions[i];

		//Create an entity to represent our wall
		Entity* pWallEntity = new Entity();

		//Create a transfrom so our wall has position and give it our wall psoition
		TransformComponent* pWallTransform = new TransformComponent(pWallEntity);
		pWallTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, currentWallPosition);

		//Create a our collider to our entities can collider with this
		ColliderComponent* pWallCollider = new ColliderComponent(pWallEntity, m_pSceneCollisionWorld);
		pWallCollider->AddBoxCollider(currentWallSize, glm::vec3(0, 0, 0));

		//Add components to our wall entity
		pWallEntity->AddComponent(pWallTransform);
		pWallEntity->AddComponent(pWallCollider);
		
	}
	
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

	const float xoffset = xpos - pScene->m_fLastX;
	const float yoffset = pScene->m_fLastY - ypos; // reversed since y-coordinates go from bottom to top

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
