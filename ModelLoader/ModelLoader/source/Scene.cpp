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

	//-----------------1------------------//
	
	//Create containment volume
	const glm::vec3 v3WallSize = glm::vec3(10.f, 10.f, 0.1f);
	Entity* pWall = new Entity();

	//Transform Component
	TransformComponent* pWallTransform = new TransformComponent(pWall);
	pWallTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(0,0,10));
	pWall->AddComponent(pWallTransform);

	//Collider Component
	ColliderComponent* pWallCollider = new ColliderComponent(pWall, m_pSceneCollisionWorld);
	pWallCollider->AddBoxCollider(v3WallSize, glm::vec3(0, 0, 0));
	pWall->AddComponent(pWallCollider);
	
	//Wall Component
	BoxPrimitiveComponent* pWallDraw = new BoxPrimitiveComponent(pWall, v3WallSize);
	pWall->AddComponent(pWallDraw);

	//-----------------2------------------//

	//Create containment volume
	const glm::vec3 v3WallSize2 = glm::vec3(10.f, 10.f, 0.1f);
	Entity* pWall2 = new Entity();

	//Transform Component
	TransformComponent* pWallTransform2 = new TransformComponent(pWall2);
	pWallTransform2->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(0, 0, -10));
	pWall2->AddComponent(pWallTransform2);

	//Collider Component
	ColliderComponent* pWallCollider2 = new ColliderComponent(pWall2, m_pSceneCollisionWorld);
	pWallCollider2->AddBoxCollider(v3WallSize2, glm::vec3(0, 0, 0));
	pWall2->AddComponent(pWallCollider2);

	//Wall Component
	BoxPrimitiveComponent* pWallDraw2 = new BoxPrimitiveComponent(pWall2, v3WallSize);
	pWall2->AddComponent(pWallDraw2);

	//-----------------3------------------//

	//Create containment volume
	const glm::vec3 v3WallSize3 = glm::vec3(10.f, .1f, 10.f);
	Entity* pWall3 = new Entity();

	//Transform Component
	TransformComponent* pWallTransform3 = new TransformComponent(pWall3);
	pWallTransform3->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(0, -10, 0));
	pWall3->AddComponent(pWallTransform3);

	//Collider Component
	ColliderComponent* pWallCollider3 = new ColliderComponent(pWall3, m_pSceneCollisionWorld);
	pWallCollider3->AddBoxCollider(v3WallSize3, glm::vec3(0, 0, 0));
	pWall3->AddComponent(pWallCollider3);

	//Wall Component
	BoxPrimitiveComponent* pWallDraw3 = new BoxPrimitiveComponent(pWall3, v3WallSize3);
	pWall3->AddComponent(pWallDraw3);

	//-----------------4------------------//

	//Create containment volume
	const glm::vec3 v3WallSize4 = glm::vec3(10.f, .1f, 10.f);
	Entity* pWall4 = new Entity();

	//Transform Component
	TransformComponent* pWallTransform4 = new TransformComponent(pWall4);
	pWallTransform4->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(0, 10, 0));
	pWall4->AddComponent(pWallTransform4);

	//Collider Component
	ColliderComponent* pWallCollider4 = new ColliderComponent(pWall4, m_pSceneCollisionWorld);
	pWallCollider4->AddBoxCollider(v3WallSize4, glm::vec3(0, 0, 0));
	pWall4->AddComponent(pWallCollider4);

	//Wall Component
	BoxPrimitiveComponent* pWallDraw4 = new BoxPrimitiveComponent(pWall4, v3WallSize4);
	pWall4->AddComponent(pWallDraw4);

	//-----------------5------------------//

	//Create containment volume
	const glm::vec3 v3WallSize5 = glm::vec3(.1f, 10.f, 10.f);
	Entity* pWall5 = new Entity();

	//Transform Component
	TransformComponent* pWallTransform5 = new TransformComponent(pWall5);
	pWallTransform5->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(10, 0, 0));
	pWall5->AddComponent(pWallTransform5);

	//Collider Component
	ColliderComponent* pWallCollider5 = new ColliderComponent(pWall5, m_pSceneCollisionWorld);
	pWallCollider5->AddBoxCollider(v3WallSize5, glm::vec3(0, 0, 0));
	pWall5->AddComponent(pWallCollider5);

	//Wall Component
	BoxPrimitiveComponent* pWallDraw5 = new BoxPrimitiveComponent(pWall5, v3WallSize5);
	pWall5->AddComponent(pWallDraw5);

	//-----------------6------------------//

	//Create containment volume
	const glm::vec3 v3WallSize6 = glm::vec3(.1f, 10.f, 10.f);
	Entity* pWall6 = new Entity();

	//Transform Component
	TransformComponent* pWallTransform6 = new TransformComponent(pWall6);
	pWallTransform6->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, glm::vec3(-10, 0, 0));
	pWall6->AddComponent(pWallTransform6);

	//Collider Component
	ColliderComponent* pWallCollider6 = new ColliderComponent(pWall6, m_pSceneCollisionWorld);
	pWallCollider6->AddBoxCollider(v3WallSize6, glm::vec3(0, 0, 0));
	pWall5->AddComponent(pWallCollider6);

	//Wall Component
	BoxPrimitiveComponent* pWallDraw6 = new BoxPrimitiveComponent(pWall6, v3WallSize6);
	pWall5->AddComponent(pWallDraw6);

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
rp3d::CollisionWorld* Scene::GetCollisionWorld() const
{
	return m_pSceneCollisionWorld;
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
