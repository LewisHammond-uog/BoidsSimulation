#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

//Project Includes
#include "Component.h"

//GLM Includes
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

//Forward Declare
struct GLFWwindow;
class TransformComponent;



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class CameraMove {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class CameraComponent : public Component
{
public:

	//Constructor
	CameraComponent(Entity* a_pOwner, GLFWwindow* a_pWindow, glm::vec3 a_v3Pos = sc_v3DefaultPosition, glm::vec3 a_v3Up = sc_v3DefaultUp, float a_fYaw = sc_fDefaultYaw, float a_fPitch = sc_fDefaultPitch);

	//Reqiured Update/Draw Functions
	virtual void Update(float a_fDeltaTime) override;
	virtual void Draw(Shader* a_pShader){};

	glm::mat4 GetViewMatrix() const;
	float GetCameraZoom() const;

	//Process Functions
	void ProcessMouseMovement(float a_fXOffset, float a_fYOffset, bool a_constrainPitch = true);
	void ProcessMouseScroll(float a_fYOffset);
	
private:


	void UpdateCameraVectors() const;

	//Input Processing
	void ProcessInput(float a_fDeltaTime);
	void ProcessKeyboard(CameraMove a_eDirection, float a_fDeltaTime) const;

	//Pointer to our transform
	TransformComponent* m_pCameraTransform;

	//Window that this camera if for
	GLFWwindow* m_pWindow;
	
	//Camera Attributes
	glm::vec3 m_v3WorldUp;
	float m_fYaw;
	float m_fPitch;
	//Camera settings
	float m_fMovementSpeed;
	float m_fMouseSensitivity;
	float m_fZoom;

	#pragma region Camera Defaults
	//Default Camera values
	const static glm::vec3 sc_v3DefaultPosition;
	const static glm::vec3 sc_v3DefaultUp;
	const static glm::vec3 sc_v3DefaultForward;
	const static float sc_fDefaultYaw;
	const static float sc_fDefaultPitch;
	const static float sc_fDefaultSpeed;
	const static float sc_fDefaultSensitivity;
	const static float sc_fDefaultZoom;
	#pragma endregion
};

#endif // CAMERA_COMPONENT_H