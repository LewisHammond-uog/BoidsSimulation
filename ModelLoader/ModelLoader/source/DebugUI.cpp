#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//Project Includes
#include <iostream>

#include "Scene.h"

//Init Singleton var
DebugUI* DebugUI::s_pUIInstance = nullptr;

//Default Values
const float mc_fDefaultForce = 0.0f;

///Get singleton instance of UI
DebugUI* DebugUI::GetInstance() {

	//Create an instance if we don't have one
	if (s_pUIInstance == nullptr) {
		s_pUIInstance = new DebugUI();
	}

	return s_pUIInstance;
}

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
			Scene::GetInstance()->DeInitlise(false);
			Scene::GetInstance()->Initalise(false);
			std::cout << "\n SCENE RESTARTED \n" << std::endl;
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
		ImGui::SliderFloat("Neighbourhood Radius", &m_uiValues.fInputNeighbourRadius, 1.0f, 20.f);

		ImGui::Spacing();
		
		//Collision Avoidance Forces
		ImGui::Text("Collision Avoidance");
		ImGui::SliderFloat("Containment Force Weight", &m_uiValues.fInputContainmentForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Collision Avoidance Force Weight", &m_uiValues.fInputCollisionAvoidForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		
		ImGui::Spacing();

		//Flocking Forces
		ImGui::Text("Flocking Forces");
		ImGui::SliderFloat("Seperation Force Weight", &m_uiValues.fInputSeperationForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Alignment Force Weight", &m_uiValues.fInputAlignmentForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Cohension Force Weight", &m_uiValues.fInputCohesionForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Wander Force Weight", &m_uiValues.fInputWanderForce, mc_fMinForceWeight, mc_fMaxForceWeight);

		if(ImGui::CollapsingHeader("Wander Settings"))
		{
			ImGui::Text("Settings for the sphere that is projected in front of the sphere.");
			
			ImGui::SliderFloat("Sphere Forward Projection", &m_uiValues.fInputWanderForward, 0.f, 5.f);
			ImGui::SliderFloat("Sphere Jitter", &m_uiValues.fInputWanderJitter, 0.01f, 5.f);
			ImGui::SliderFloat("Sphere Radius", &m_uiValues.fInputWanderRadius, 0.01f, 5.f);
		}
	}

	if(ImGui::CollapsingHeader("World Settings"))
	{
		ImGui::Text("Adjust the world bounds size (Requires Scene Restart)");
		ImGui::SliderInt("World Bounds Size", &m_uiValues.iInputWorldBounds, 10.f, 50.f);
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

DebugUI::DebugUI()
{
}

DebugUI::~DebugUI()
{
}
