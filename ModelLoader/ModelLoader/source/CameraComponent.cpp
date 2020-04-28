#include "CameraComponent.h"

//Project Includes
#include <GLFW/glfw3.h>

#include "Entity.h"
#include "TransformComponent.h"

//Typedefs
typedef Component PARENT;

//Declare Camera Statics - default values
const glm::vec3 CameraComponent::sc_v3DefaultPosition = glm::vec3(0.f);
const glm::vec3 CameraComponent::sc_v3DefaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 CameraComponent::sc_v3DefaultForward = glm::vec3(0.0f, 0.0f, -1.0f);
const float CameraComponent::sc_fDefaultYaw = -90.f;
const float CameraComponent::sc_fDefaultPitch = 0.0f;
const float CameraComponent::sc_fDefaultSpeed = 2.5f;
const float CameraComponent::sc_fDefaultSensitivity = 0.1f;
const float CameraComponent::sc_fDefaultZoom = 45.f;

CameraComponent::CameraComponent(Entity* a_pOwner, GLFWwindow* a_pWindow, glm::vec3 a_v3Pos, glm::vec3 a_v3Up, float a_fYaw, float a_fPitch) :
	PARENT(a_pOwner),
	m_pWindow(a_pWindow),
	m_fYaw(a_fYaw),
	m_fPitch(a_fPitch),
	m_fMovementSpeed(sc_fDefaultSpeed),
	m_fZoom(sc_fDefaultZoom),
	m_fMouseSensitivity(sc_fDefaultSensitivity),
	m_v3WorldUp(a_v3Up)
{
	//Get our Transform and apply position and up values
	if(a_pOwner)
	{
		m_pCameraTransform = a_pOwner->GetComponent<TransformComponent*>();
		if(m_pCameraTransform)
		{
			m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, a_v3Pos);
			m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::UP_VECTOR, a_v3Up);
			m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR, sc_v3DefaultForward);
		}
	}
}

/// <summary>
/// Update the camera by processsing input
/// </summary>
/// <param name="a_fDeltaTime"></param>
void CameraComponent::Update(float a_fDeltaTime)
{
	//Process the input that we have received
	ProcessInput(a_fDeltaTime);
}

/// <summary>
/// Gets the view Matrix of the Camera
/// </summary>
/// <returns>Camera View Matrix</returns>
glm::mat4 CameraComponent::GetViewMatrix() const
{
	if (!m_pCameraTransform)
	{
		return glm::mat4();
	}
	
	const glm::vec3 v3CurrentPos = m_pCameraTransform->GetCurrentPosition();
	const glm::vec3 v3Forward = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	const glm::vec3 v3Up = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);
	return glm::lookAt(v3CurrentPos, v3CurrentPos + v3Forward, v3Up);
}

float CameraComponent::GetCameraZoom() const
{
	return m_fZoom;
}

void CameraComponent::ProcessMouseMovement(float a_fXOffset, float a_fYOffset, bool a_constrainPitch)
{
	a_fXOffset *= m_fMouseSensitivity;
	a_fYOffset *= m_fMouseSensitivity;

	m_fYaw += a_fXOffset;
	m_fPitch += a_fYOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (a_constrainPitch)
	{
		if (m_fPitch > 89.0f)
			m_fPitch = 89.0f;
		if (m_fPitch < -89.0f)
			m_fPitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	UpdateCameraVectors();
}

void CameraComponent::ProcessMouseScroll(float a_fYOffset)
{
	if (m_fZoom >= 1.0f && m_fZoom <= 45.0f)
		m_fZoom -= a_fYOffset;
	if (m_fZoom <= 1.0f)
		m_fZoom = 1.0f;
	if (m_fZoom >= 45.0f)
		m_fZoom = 45.0f;
}

/// <summary>
/// Updates the camera forward based on the cameras euler angles
/// </summary>
void CameraComponent::UpdateCameraVectors() const
{
	if(!m_pCameraTransform)
	{
		return;
	}

	//Exisitng Right and Up Vecots
	glm::vec3 v3Forward = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3Up = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);
	glm::vec3 v3Right = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	
	//Calculate the new forward
	glm::vec3 v3NewForward;
	v3NewForward.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
	v3NewForward.y = sin(glm::radians(m_fPitch));
	v3NewForward.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
	m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR, glm::normalize(v3NewForward));
	m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR, glm::normalize(glm::cross(v3Forward, m_v3WorldUp)));
	m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::UP_VECTOR, glm::normalize(glm::cross(v3Right, v3Forward)));
}

void CameraComponent::ProcessKeyboard(CameraMove direction, float deltaTime) const
{
	if(!m_pCameraTransform)
	{
		return;
	}
	
	glm::vec3 v3CameraPos = m_pCameraTransform->GetCurrentPosition();
	const glm::vec3 v3Right = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	const glm::vec3 v3Forward = m_pCameraTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);

	//Calculate new position
	const float fVelocity = m_fMovementSpeed * deltaTime;
	if (direction == CameraMove::FORWARD)
		v3CameraPos += v3Forward * fVelocity;
	if (direction == CameraMove::BACKWARD)
		v3CameraPos -= v3Forward * fVelocity;
	if (direction == CameraMove::LEFT)
		v3CameraPos -= v3Right * fVelocity;
	if (direction == CameraMove::RIGHT)
		v3CameraPos += v3Right * fVelocity;

	//Apply new position
	m_pCameraTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, v3CameraPos);
}

/// <summary>
/// Processes Input directly and polls the keys that the user has pressed
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
void CameraComponent::ProcessInput(const float a_fDeltaTime)
{
	if (m_pWindow) {
		if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
			ProcessKeyboard(CameraMove::FORWARD, a_fDeltaTime);
		if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
			ProcessKeyboard(CameraMove::BACKWARD, a_fDeltaTime);
		if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
			ProcessKeyboard(CameraMove::LEFT, a_fDeltaTime);
		if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
			ProcessKeyboard(CameraMove::RIGHT, a_fDeltaTime);
	}
}