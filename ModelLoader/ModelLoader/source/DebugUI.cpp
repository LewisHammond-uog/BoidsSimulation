#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//c++ Includes
#include <iostream>

//Project Includes
#include "BoidSpawner.h"
#include "Scene.h"

//Default Values
const float mc_fDefaultForce = 0.0f;

/// <summary>
/// Updates the UI by passing ImGUI elements to draw
/// </summary>
void DebugUI::Update() {

	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(m_v2WindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(m_v2WindowSize, ImGuiCond_FirstUseEver);

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
		if(ImGui::CollapsingHeader("Wander Settings"))
		{
			ImGui::Text("Settings for the sphere that is projected in front of the sphere.");
			
			ImGui::SliderFloat("Sphere Forward Projection", &m_uiValues.fInputWanderForward.value, m_uiValues.fInputWanderForward.min, m_uiValues.fInputWanderForward.max);
			ImGui::SliderFloat("Sphere Jitter", &m_uiValues.fInputWanderJitter.value, m_uiValues.fInputWanderJitter.min, m_uiValues.fInputWanderJitter.max);
			ImGui::SliderFloat("Sphere Radius", &m_uiValues.fInputWanderRadius.value, m_uiValues.fInputWanderRadius.min, m_uiValues.fInputWanderRadius.max);
		}

		
	}

	//World Settings
	if(ImGui::CollapsingHeader("World Settings"))
	{
		ImGui::Text("Adjust the world bounds size (Requires Scene Restart)");
		
		ImGui::SliderInt("World Bounds Size", &m_uiValues.iInputWorldBounds.value, 10.f, 50.f);
		//Boid Count
		if(ImGui::SliderInt("Boid Count", &m_uiValues.iBoidCount.value, 1.f, 250.f))
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