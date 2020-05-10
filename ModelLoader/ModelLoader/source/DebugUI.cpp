#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//c++ Includes
#include <iostream>

//Project Includes
#include "BoidSpawner.h"
#include "Scene.h"
#include "Entity.h"

/// <summary>
/// Updates the UI by passing ImGUI elements to draw
/// </summary>
void DebugUI::Update() {

	//Draw general UI
	DrawDebugUI();
	
	//Draw Inspector Window
	DrawInspector();
}

/// <summary>
/// Gets all of the values the user has input to the UI
/// </summary>
/// <returns></returns>
UIInputValues* DebugUI::GetUIInputValues()
{
	return &m_uiValues;
}


/// <summary>
/// Gets if the UI has the mouse hovered, clicked or
/// otherwise interacting with
/// </summary>
/// <returns>If mouse is focused on the UI</returns>
bool DebugUI::HasMouseFocus()
{
	return ImGui::GetIO().WantCaptureMouse;
}

/// <summary>
/// Draw the debug UI
/// </summary>
void DebugUI::DrawDebugUI()
{
	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(m_v2DebugWindowPos, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(m_v2DebugWindowSize, ImGuiCond_FirstUseEver);

	//Begin the drawing of the Window
	ImGui::Begin("Boids UI");

	//FPS info tab
	if (ImGui::CollapsingHeader("Game Info")) {
		ImGui::Text("Application Average: %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Spacing();

		//Tickbox to draw colliders
		ImGui::Checkbox("Draw Collider Bounds", &m_uiValues.bShowColliders);

		ImGui::Spacing();

		//Button to Restart the scene
		if (ImGui::Button("Restart Scene"))
		{
			ImGui::Text("Restarting...");
			Scene::GetInstance()->DeInitialize(false);
			Scene::GetInstance()->Initialize(false);
		}
	}

	if (ImGui::CollapsingHeader("Controls")) {
		//Controls Text
		ImGui::Text("Controls");
		ImGui::Text("WASD to Move the Camera");
		ImGui::Text("Left Click to Rotate the Camera");
		ImGui::Text("Hold and Release Right Click to Place an Obstacle");
	}

	//Sliders for changing force weights - UI puts these values in to the appropriate varables as
	//we pass by ref
	if (ImGui::CollapsingHeader("Global Boid Values")) {
		ImGui::Text("Detection Values");

		//Nehbourhood Radius
		ImGui::SliderFloat("Neighbourhood Radius", &m_uiValues.fInputNeighbourRadius.value, m_uiValues.fInputNeighbourRadius.min, m_uiValues.fInputNeighbourRadius.max);

		ImGui::Spacing();

		//Collision Avoidance Forces
		ImGui::Text("Collision Avoidance");
		ImGui::SliderFloat("Containment Force Weight", &m_uiValues.fInputContainmentForce.value, m_uiValues.fInputContainmentForce.min, m_uiValues.fInputContainmentForce.max);
		ImGui::SliderFloat("Collision Avoidance Force Weight", &m_uiValues.fInputCollisionAvoidForce.value, m_uiValues.fInputCollisionAvoidForce.min, m_uiValues.fInputCollisionAvoidForce.max);

		ImGui::Spacing();

		//Flocking Forces
		ImGui::Text("Flocking Forces");
		ImGui::SliderFloat("Separation Force Weight", &m_uiValues.fInputSeparationForce.value, m_uiValues.fInputSeparationForce.min, m_uiValues.fInputSeparationForce.max);
		ImGui::SliderFloat("Alignment Force Weight", &m_uiValues.fInputAlignmentForce.value, m_uiValues.fInputAlignmentForce.min, m_uiValues.fInputAlignmentForce.max);
		ImGui::SliderFloat("Cohesion Force Weight", &m_uiValues.fInputCohesionForce.value, m_uiValues.fInputCohesionForce.min, m_uiValues.fInputCohesionForce.max);
		ImGui::SliderFloat("Wander Force Weight", &m_uiValues.fInputWanderForce.value, m_uiValues.fInputWanderForce.min, m_uiValues.fInputWanderForce.max);

		//Wander Settings
		if (ImGui::CollapsingHeader("Wander Settings"))
		{
			ImGui::Text("Settings for the sphere that is projected in front of the sphere.");

			ImGui::SliderFloat("Sphere Forward Projection", &m_uiValues.fInputWanderForward.value, m_uiValues.fInputWanderForward.min, m_uiValues.fInputWanderForward.max);
			ImGui::SliderFloat("Sphere Jitter", &m_uiValues.fInputWanderJitter.value, m_uiValues.fInputWanderJitter.min, m_uiValues.fInputWanderJitter.max);
			ImGui::SliderFloat("Sphere Radius", &m_uiValues.fInputWanderRadius.value, m_uiValues.fInputWanderRadius.min, m_uiValues.fInputWanderRadius.max);
		}
	}

	//World Settings
	if (ImGui::CollapsingHeader("World Settings"))
	{
		ImGui::Text("Adjust the world bounds size (Requires Scene Restart)");

		ImGui::SliderInt("World Bounds Size", &m_uiValues.iInputWorldBounds.value, m_uiValues.iInputWorldBounds.min, m_uiValues.iInputWorldBounds.max);
		//Boid Count
		if (ImGui::SliderInt("Boid Count", &m_uiValues.iBoidCount.value, m_uiValues.iBoidCount.min, m_uiValues.iBoidCount.max))
		{
			//This is triggered when our boid count slider changes, when this happens
			//we call the boid spawner to adjust the number of boids
			BoidSpawner::GetInstance()->AdjustBoidCount(m_uiValues.iBoidCount.value);
		}
	}

	//End the drawing of the window
	ImGui::End();
}

/// <summary>
/// Draw an inspector window to show boid infomation
/// </summary>
void DebugUI::DrawInspector() const
{
	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(m_v2InspectorPos, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(m_v2InspectorSize, ImGuiCond_FirstUseEver);

	//Begin the drawing of the Window
	ImGui::Begin("Entity Inspector");

	//Selector for boid
	static int iSelectedEntityID = 0;
	ImGui::InputInt("Boid ID:", &iSelectedEntityID);

	//Get entity info
	Entity* pEntity = Entity::GetEntityMap().find(iSelectedEntityID)->second;
	if (pEntity != nullptr) {

		//Show Entity ID Type
		ImGui::Text("Entity ID: %i", pEntity->GetEntityID());
		ImGui::Text("Entity Type: %s", pEntity->GetEntityTypeName());
		
		ImGui::Spacing();
		
		//Show Transform
		TransformComponent* pBoidTransform = pEntity->GetComponent<TransformComponent*>();
		if(pBoidTransform){
			const glm::vec3 v3BoidPos = pEntity->GetComponent<TransformComponent*>()->GetCurrentPosition();
			const glm::vec3 v3BoidDir = pEntity->GetComponent<TransformComponent*>()->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR) * 360.f;

			ImGui::Text("Position: (%f,%f,%f)", v3BoidPos.x, v3BoidPos.y, v3BoidPos.z);
			ImGui::Text("Direction: (%f,%f,%f)", v3BoidDir.x, v3BoidDir.y, v3BoidDir.z);
		}

		//List out all of the components that we have
		ImGui::Spacing();
		ImGui::Text("Components:");
		std::vector<Component*> m_vEntityComponents = pEntity->GetComponentList();
		for(int i = 0; i < m_vEntityComponents.size(); ++i)
		{
			ImGui::Text(m_vEntityComponents[i]->GetComponentName());
		}
		
	}else
	{
		//No Entity - show error
		ImGui::Text("No Entity exists with ID %i", iSelectedEntityID);
	}
	
	ImGui::End();
}
