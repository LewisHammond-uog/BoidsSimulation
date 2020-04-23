#ifndef __SCENE_H__
#define __SCENE_H__

//Parent Class include
#include "Application.h"

//React Physics Includes
#include <ReactPhysics3D/reactphysics3d.h>


//Foward Declares
class Camera;
class CameraComponent;
class Shader;
class Model;

class Scene : private Application
{
public:
	static Scene* GetInstance();

	bool Initalise();
	bool Update();
	void Render();
	void DeInitlise();

	//Get the collision world - used for collider components
	rp3d::CollisionWorld* GetCollisionWorld() const;

private:
	Scene();
	Scene(const Scene&); //Overload Copy Constructor - no implementation
	Scene& operator=(const Scene&); //Overload Equals Operator - no implementation

	//Function to generate our bounds volume
	void GenerateBoundsVolume(float a_fBoundsSize) const;

	//Function to create an obsticle
	void CreateObstacle(glm::vec3 a_v3Position);
	
	//OpenGL Callback functions
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	CameraComponent* m_pCamera;
	Shader* m_ourShader;
	Model* m_pNanoSuitModel;

	//Collision World
	rp3d::CollisionWorld* m_pSceneCollisionWorld;

	//Mouse Info
	float m_fLastX; //Last X Position of the mouse
	float m_fLastY; //Last Y Position of the mouse
	bool m_bFirstMouse; //If this is the first time that we are geting mouse info
	
	static Scene* s_pSceneInstance; //Single instance of this scene;
};


#endif // !__SCENE_H__
