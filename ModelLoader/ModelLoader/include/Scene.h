#ifndef __SCENE_H__
#define __SCENE_H__

//Parent Class include
#include "Application.h"

//React Physics Includes
#include <vector>
#include <ReactPhysics3D/reactphysics3d.h>


//Foward Declares
class Camera;
class CameraComponent;
class Shader;
class Model;

class Scene : private Application
{
public:
	Scene(const Scene&) = delete; //Overload Copy Constructor - no implementation
	Scene& operator=(const Scene&) = delete; //Overload Equals Operator - no implementation
	
	static Scene* GetInstance();

	bool Initialize(bool a_bInitApplication);
	bool Update() override;
	void Render() override;
	void DeInitialize(bool a_bCloseApplication);

	//Get the collision world - used for collider components
	rp3d::CollisionWorld* GetCollisionWorld() const;

private:
	Scene();
	
	//Function to generate our bounds volume
	void GenerateBoundsVolume(float a_fBoundsSize) const;
	
	//OpenGL Callback functions
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void toggle_mouse_input_mode(GLFWwindow* window, int button, int action, int mods);

	CameraComponent* m_pCamera;
	Shader* m_ourShader;

	//Collision World
	rp3d::CollisionWorld* m_pSceneCollisionWorld;

	//Mouse Info
	float m_fLastX; //Last X Position of the mouse
	float m_fLastY; //Last Y Position of the mouse
	bool m_bFirstMouse; //If this is the first time that we are geting mouse info
	
	static Scene* s_pSceneInstance; //Single instance of this scene;
};


#endif // !__SCENE_H__
