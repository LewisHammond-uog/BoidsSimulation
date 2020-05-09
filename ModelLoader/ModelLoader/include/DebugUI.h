#ifndef __DEBUG_UI_H__
#define __DEBUG_UI_H__

//Imgui Includes
#include <imgui/imgui.h>

//Enum for types of flocking values
typedef enum class ForceWeight {
	FORCE_WEIGHT_CONTAINMENT,
	FORCE_WEIGHT_COLLISION_AVOID,
	FORCE_WEIGHT_Separation,
	FORCE_WEIGHT_ALIGNMENT,
	FORCE_WEIGHT_COHESION,
	FORCE_WEIGHT_WANDER,

	FORCE_WEIGHT_COUNT //Number of force weights
}FlockingBehaviourType;

// ReSharper disable CppInconsistentNaming

/// <summary>
/// Struct for storing all of the values that the user has input
/// in to the UI
/// </summary>
struct UIInputValues
{
public:
	//FORCES

	float fInputContainmentForce = 5.f;
	float fInputCollisionAvoidForce = 0.f;
	float fInputSeparationForce = 0.f;
	float fInputAlignmentForce = 0.f;
	float fInputCohesionForce = 0.f;
	float fInputWanderForce = 0.f;
	//WANDER SETTINGS
	float fInputWanderForward = 2.f;
	float fInputWanderJitter = 0.05f;
	float fInputWanderRadius = 0.33f;
	//NEIGHBOUR RADIUSS
	float fInputNeighbourRadius = 5.0f;
	//WORLD SETTINGS
	int iInputWorldBounds = 10;
	//DEBUG
	bool bShowColliders = false;
};

// ReSharper restore CppInconsistentNaming

class DebugUI {

public:

	//Funct to get the singeton instance
	//of the debug UI
	static DebugUI* GetInstance();

	void Update();

	//Get all of the UI Values
	UIInputValues* GetUIInputValues();

	//Get if this UI has the mouse in focus
	static bool HasMouseFocus();
	
private:
	//Private Constructors/Destructors
	//that prevent duplicaton of the singleton
	DebugUI() = default;
	~DebugUI() = default;

	//Instance of the debug UI
	static DebugUI* s_pUIInstance; //Single instance of this UI

	//UI Position
	const ImVec2 m_v2WindowPos = ImVec2(0, 0);
	const ImVec2 m_v2WindowSize = ImVec2(300, 600);

	//Max and min values
	const float mc_fMinForceWeight = 0.0f;
	const float mc_fMaxForceWeight = 5.0f;

	//All of the UI Values
	UIInputValues m_uiValues;

};

#endif // ! __DEBUG_UI_H__

