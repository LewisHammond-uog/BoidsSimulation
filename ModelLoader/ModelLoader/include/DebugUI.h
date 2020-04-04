#ifndef __DEBUG_UI_H__
#define __DEBUG_UI_H__

//Imgui Includes
#include <imgui/imgui.h>

//Enum for types of flocking values
typedef enum FlockingBehaviourType {
	BEHAVIOUR_SEPERATION,
	BEHAVIOUR_ALIGNMENT,
	BEHAVIOUR_COHESION
}FlockingBehaviourType;

class DebugUI {

public:

	//Funct to get the singeton instance
	//of the debug UI
	static DebugUI* GetInstance();

	void Update();

	//Function to get value about flocking
	//that the user has input in to the UI
	float GetUIFlockingWeight(FlockingBehaviourType a_eBehaviourType) const;
	bool GetShowColliders() const;

private:
	//Private Constructors/Destructors
	//that prevent duplicaton of the singleton
	DebugUI();
	~DebugUI();

	//Instance of the debug UI
	static DebugUI* s_pUIInstance; //Single instance of this UI

	//UI Position
	const ImVec2 m_v2WindowPos = ImVec2(0, 0);
	const ImVec2 m_v2WindowSize = ImVec2(300, 300);

	//Max and min values
	const float mc_fMinForceWeight = 0.0f;
	const float mc_fMaxForceWeight = 5.0f;

	//Variables for different game elements that are controlled
	//by the UI
	float m_fInputSeperationForce;
	float m_fInputAlignmentForce;
	float m_fInputCohesionForce;
	bool m_bShowColliders;

};

#endif // ! __DEBUG_UI_H__

