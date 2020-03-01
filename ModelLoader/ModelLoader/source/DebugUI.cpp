#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//Init Singleton var
DebugUI* DebugUI::s_pUIInstance = nullptr;

//Default Values
const float mc_fDefaultForce = 0.0f;

///Get singleton instance of scene
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
	ImGui::Begin("Flocking Values");

	//Sliders for changing force weights - UI puts these values in to the appropriate varables as
	//we pass by ref
	ImGui::SliderFloat("Seperation Force Weight", &m_fInputSeperationForce, mc_fMinForceWeight, mc_fMaxForceWeight); //Not Implemented
	ImGui::SliderFloat("Alignment Force Weight", &m_fInputAlignmentForce, mc_fMinForceWeight, mc_fMaxForceWeight); //Not Implemented
	ImGui::SliderFloat("Cohension Force Weight", &m_fInputCohesionForce, mc_fMinForceWeight, mc_fMaxForceWeight); //Not Implemented

	//End the drawing of the window
	ImGui::End();
}

/// <summary>
/// Get the value that the user has input for the given
/// type of flocking behaviour
/// </summary>
/// <param name="a_eBehaviourType">Behvaiour type of get value for</param>
/// <returns>Weight of Behvaiour</returns>
float DebugUI::GetUIFlockingWeight(FlockingBehaviourType a_eBehaviourType)
{
	//TODO MAKE THIS RETURN A HASH MAP

	//Switch through the different behaviour types
	//and return the appropriate value
	switch (a_eBehaviourType)
	{
	case BEHAVIOUR_SEPERATION:
		return m_fInputSeperationForce;
		break;
	case BEHAVIOUR_ALIGNMENT:
		return m_fInputAlignmentForce;
		break;
	case BEHAVIOUR_COHESION:
		return m_fInputCohesionForce;
		break;
	default:
		//Default (i.e invalid value), return 0 so that
		//we don't apply that behaviour
		return 0.f;
		break;
	}
}

DebugUI::DebugUI() :
	m_fInputSeperationForce(mc_fDefaultForce),
	m_fInputCohesionForce(mc_fDefaultForce),
	m_fInputAlignmentForce(mc_fDefaultForce)
{
}

DebugUI::~DebugUI()
{
}
