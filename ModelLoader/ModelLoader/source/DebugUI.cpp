#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
	if (ImGui::CollapsingHeader("Debug/FPS info")) {
		ImGui::Text("Application Average: %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//Tickbox to draw colliders
		ImGui::Checkbox("Draw Collider Bounds", &m_bShowColliders);
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
	if (ImGui::CollapsingHeader("Force Values")) {
		ImGui::Text("Simulation Values");
		ImGui::SliderFloat("Containment Force Weight", &m_fInputContainmentForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Collision Avoidance Force Weight", &m_fInputCollisionAvoidForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Seperation Force Weight", &m_fInputSeperationForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Alignment Force Weight", &m_fInputAlignmentForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Cohension Force Weight", &m_fInputCohesionForce, mc_fMinForceWeight, mc_fMaxForceWeight);
		ImGui::SliderFloat("Wander Force Weight", &m_fInputWanderForce, mc_fMinForceWeight, mc_fMaxForceWeight);
	}

	//End the drawing of the window
	ImGui::End();
}

/// <summary>
/// Get the value that the user has input for the given
/// type of flocking behaviour
/// </summary>
/// <param name="a_eBehaviourType">Behvaiour type of get value for</param>
/// <returns>Weight of Behvaiour</returns>
float DebugUI::GetUIFlockingWeight(const ForceWeight a_eBehaviourType) const
{
	//TODO MAKE THIS RETURN A HASH MAP so that we only request it once

	//Switch through the different behaviour types
	//and return the appropriate value
	switch (a_eBehaviourType)
	{
	case ForceWeight::FORCE_WEIGHT_CONTAINMENT:
		return m_fInputContainmentForce;
		break;
	case ForceWeight::FORCE_WEIGHT_COLLISION_AVOID:
		return m_fInputCollisionAvoidForce;
		break;
	case ForceWeight::FORCE_WEIGHT_SEPERATION:
		return m_fInputSeperationForce;
		break;
	case ForceWeight::FORCE_WEIGHT_ALIGNMENT:
		return m_fInputAlignmentForce;
		break;
	case ForceWeight::FORCE_WEIGHT_COHESION:
		return m_fInputCohesionForce;
		break;
	case ForceWeight::FORCE_WEIGHT_WANDER:
		return m_fInputWanderForce;
		break;
	default:
		//Default (i.e invalid value), return 0 so that
		//we don't apply that behaviour
		return 0.f;
		break;
	}
}

/// <summary>
/// Get if should draw colliders on our entities
/// </summary>
/// <returns></returns>
bool DebugUI::GetShowColliders() const
{
	return m_bShowColliders;
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

DebugUI::DebugUI() :
	m_fInputContainmentForce(mc_fDefaultForce),
	m_fInputCollisionAvoidForce(mc_fDefaultForce),
	m_fInputSeperationForce(mc_fDefaultForce),
	m_fInputAlignmentForce(mc_fDefaultForce),
	m_fInputCohesionForce(mc_fDefaultForce),
	m_fInputWanderForce(mc_fDefaultForce),
	m_bShowColliders(false)
{
}

DebugUI::~DebugUI()
{
}
