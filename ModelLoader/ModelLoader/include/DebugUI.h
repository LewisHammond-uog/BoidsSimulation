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
public:
	//FORCES									//RANGE(start, min, max)	
	UIRange<float> fInputContainmentForce		= UIRange<float>(5.f, 0.f, 5.f);
	UIRange<float> fInputCollisionAvoidForce	= UIRange<float>(5.f, 0.f, 5.f);
	UIRange<float> fInputSeparationForce		= UIRange<float>(0.f, 0.f, 5.f);
	UIRange<float> fInputAlignmentForce			= UIRange<float>(0.f, 0.f, 5.f);
	UIRange<float> fInputCohesionForce			= UIRange<float>(0.f, 0.f, 5.f);
	UIRange<float> fInputWanderForce			= UIRange<float>(0.f, 0.f, 5.f);
	//WANDER SETTINGS
	UIRange<float> fInputWanderForward			= UIRange<float>(2.f, 0.f, 5.f);
	UIRange<float> fInputWanderJitter			= UIRange<float>(0.05f, 0.f, 0.2f);
	UIRange<float> fInputWanderRadius			= UIRange<float>(0.33f, 0.f, 1.f);
	//NEIGHBOUR RADIUSS
	UIRange<float> fInputNeighbourRadius		= UIRange<float>(0.f, 0.f, 5.f);
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

	//UI Position
	const ImVec2 m_v2WindowPos = ImVec2(0, 0);
	const ImVec2 m_v2WindowSize = ImVec2(300, 600);

	//All of the UI Values
	UIInputValues m_uiValues;

};

#endif // ! __DEBUG_UI_H__

