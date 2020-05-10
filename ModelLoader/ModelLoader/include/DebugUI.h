#ifndef __DEBUG_UI_H__
#define __DEBUG_UI_H__

//Imgui Includes
#include <imgui/imgui.h>

#include "Singleton.h"

/// <summary>
/// Struct for values that are used in the UI
/// Containts the current value, minimum and maximum
/// </summary>
template <class T>
struct UIRange
{
	UIRange(const T a_startValue, const  T a_minValue, const  T a_maxValue) :
		value(a_startValue), min(a_minValue), max(a_maxValue) {};

	T value; //Current Value
	const T min;
	const T max;
};

/// <summary>
/// Struct for storing all of the values that the user has input
/// in to the UI
/// </summary>
struct UIInputValues
{
	//FORCES									//RANGE(start/default, min, max)	
	UIRange<float> fInputContainmentForce		= UIRange<float>(1.10f, 0.f, 2.f);
	UIRange<float> fInputCollisionAvoidForce	= UIRange<float>(0.72f, 0.f, 2.f);
	UIRange<float> fInputSeparationForce		= UIRange<float>(1.029f, 0.f, 1.5f);
	UIRange<float> fInputAlignmentForce			= UIRange<float>(0.260f, 0.f, 1.5f);
	UIRange<float> fInputCohesionForce			= UIRange<float>(1.016f, 0.f, 1.5f);
	UIRange<float> fInputWanderForce			= UIRange<float>(0.247f, 0.f, 1.f);
	//WANDER SETTINGS
	UIRange<float> fInputWanderForward			= UIRange<float>(5.f, 1.f, 10.f);
	UIRange<float> fInputWanderJitter			= UIRange<float>(0.025f, 0.001f, 0.1f); //0.001 as we cannot have 0 jitter as we use spehere rand
	UIRange<float> fInputWanderRadius			= UIRange<float>(0.625f, 0.001f, 2.f); //0.001 as we cannot have a 0 radius sphere
	//NEIGHBOUR RADIUS
	UIRange<float> fInputNeighbourRadius		= UIRange<float>(4.f, 0.01f, 7.5f);
	//WORLD SETTINGS
	UIRange<int> iInputWorldBounds				= UIRange<int>(10, 0, 100);
	UIRange<int> iBoidCount						= UIRange<int>(25, 0, 250);;
	//DEBUG
	bool bShowColliders = false;
};

// ReSharper restore CppInconsistentNaming

class DebugUI : public Singleton<DebugUI> {
	friend class Singleton<DebugUI>; //Friend for singleton management
public:

	void Update();

	//Get all of the UI Values
	UIInputValues* GetUIInputValues();

	//Get if this UI has the mouse in focus
	static bool HasMouseFocus();
	
private:

	DebugUI() : Singleton() {};
	~DebugUI(){};

	void DrawInspector();

	//UI Position
	const ImVec2 m_v2WindowPos = ImVec2(0, 0);
	const ImVec2 m_v2InspectorPos = ImVec2(500, 0);
	const ImVec2 m_v2WindowSize = ImVec2(300, 600);

	//All of the UI Values
	UIInputValues m_uiValues;
};

#endif // ! __DEBUG_UI_H__

