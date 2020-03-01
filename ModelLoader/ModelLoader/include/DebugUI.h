#ifndef __DEBUG_UI_H__
#define __DEBUG_UI_H__

class DebugUI {

public:

	//Funct to get the singeton instance
	//of the debug UI
	static DebugUI* GetInstance();

	void Update();
	void Draw();

private:
	//Private Constructors/Destructors
	//that prevent duplicaton of the singleton
	DebugUI();
	~DebugUI();

	//Instance of the debug UI
	static DebugUI* s_pUIInstance; //Single instance of this UI

	//Variables for different game elements that are controlled
	//by the UI
	float m_fInputSeperationForce;
	float m_fInputAlignmentForce;
	float m_fInputCohensionForce;

};

#endif // ! __DEBUG_UI_H__

