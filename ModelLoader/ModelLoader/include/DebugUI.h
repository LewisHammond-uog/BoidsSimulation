#ifndef __DEBUG_UI_H__
#define __DEBUG_UI_H__

//Imgui Includes
#include <imgui/imgui.h>

//Enum for types of flocking values
typedef enum class ForceWeight {
	FORCE_WEIGHT_CONTAINMENT,
	FORCE_WEIGHT_COLLISION_AVOID,
	FORCE_WEIGHT_SEPERATION,
	FORCE_WEIGHT_ALIGNMENT,
	FORCE_WEIGHT_COHESION,
	FORCE_WEIGHT_WANDER,

	FORCE_WEIGHT_COUNT //Number of force weights
}FlockingBehaviourType;

class DebugUI {

public:

	//Funct to get the singeton instance
	//of the debug UI
	static DebugUI* GetInstance();

	void Update();

	//Function to get value about flocking
	//that the user has input in to the UI
	float GetUIFlockingWeight(ForceWeight a_eBehaviourType) const;

	float GetUINeighbourRadius();

	float GetUIWanderForward();
	float GetUIWanderJitter();
	float GetUIWanderRadius();
	
	bool GetShowColliders() const;

	//Get if this UI has the mouse in focus
	bool HasMouseFocus();
	
private:
	//Private Constructors/Destructors
	//that prevent duplicaton of the singleton
	DebugUI();
	~DebugUI();

	//Instance of the debug UI
	static DebugUI* s_pUIInstance; //Single instance of this UI

	//UI Position
	const ImVec2 m_v2WindowPos = ImVec2(0, 0);
	const ImVec2 m_v2WindowSize = ImVec2(300, 600);

	//Max and min values
	const float mc_fMinForceWeight = 0.0f;
	const float mc_fMaxForceWeight = 5.0f;

	//Variables for different game elements that are controlled
	//by the UI
	//FORCES
	float m_fInputContainmentForce = 0.f;
	float m_fInputCollisionAvoidForce = 0.f;
	float m_fInputSeperationForce = 0.f;
	float m_fInputAlignmentForce = 0.f;
	float m_fInputCohesionForce = 0.f;
	float m_fInputWanderForce = 0.f;
	//WANDER SETTINGS
	float m_fInputWanderForward = 2.f;
	float m_fInputWanderJitter = 0.05f;
	float m_fInputWanderRadius = 0.33f;
	//NEIGHBOUR RADIUSS
	float m_fInputNeighbourRadius;
	//DEBUG
	bool m_bShowColliders;

};

#endif // ! __DEBUG_UI_H__

