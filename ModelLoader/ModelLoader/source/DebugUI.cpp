#include "DebugUI.h"

//Imgui Incldues
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//Init Singleton var
DebugUI* DebugUI::s_pUIInstance = nullptr;

//Default Values
const float mc_fDefaultSeekForce = 0.0f;

///Get singleton instance of scene
DebugUI* DebugUI::GetInstance() {

	//Create an instance if we don't have one
	if (s_pUIInstance == nullptr) {
		s_pUIInstance = new DebugUI();
	}

	return s_pUIInstance;
}

void DebugUI::Update() {

	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	//Begin the drawing of the Window
	ImGui::Begin("Flocking Values");

	//Sliders and Buttons for changing force weights
	ImGui::SliderFloat("Seperation Force Weight", &m_fInputSeperationForce, 0.0f, 100.f); //Not Implemented
	ImGui::SliderFloat("Alignment Force Weight", &m_fInputAlignmentForce, 0.0f, 100.f); //Not Implemented
	ImGui::SliderFloat("Cohension Force Weight", &m_fInputCohensionForce, 0.0f, 100.f); //Not Implemented

	ImGui::End();

}

DebugUI::DebugUI() :
	m_fInputSeperationForce(mc_fDefaultSeekForce)
{
}

DebugUI::~DebugUI()
{
}
