#include "TransformComponent.h"

//Typedefs
typedef Component PARENT;

TransformComponent::TransformComponent(Entity* a_pOwner) : 
	PARENT(a_pOwner),
	m_m4EntityMatrix(glm::mat4(1.0f))
{
	m_eComponentType = TRANSFORM;
}

/// <summary>
/// Sets a row within the transform matrix
/// </summary>
/// <param name="a_eRow">Row to set</param>
/// <param name="a_v3Vec">Vector 3 to fill row with</param>
void TransformComponent::SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec)
{
	float finalVec4Value = a_eRow == POSTION_VECTOR ? 1.f : 0.0f;
	m_m4EntityMatrix[a_eRow] = glm::vec4(a_v3Vec, finalVec4Value);
}

/// <summary>
/// Gets the row of the matrix (i.e FORWARD, RIGHT, UP, POSITION)
/// </summary>
/// <param name="a_eRow">Row of the matrix to get</param>
/// <returns>Matrix Row as Vector 3</returns>
glm::vec3 TransformComponent::GetEntityMatrixRow(MATRIX_ROW a_eRow)
{
	return m_m4EntityMatrix[a_eRow];
}

///<summary>
///Gets the current position of the transform
///</summary>
///<returns>Current Transform Position</returns>
glm::vec3 TransformComponent::GetCurrentPosition()
{
	return GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);
}


void TransformComponent::Orthogonalize()
{
	//Get the forward and up of the input vector
	glm::vec3 inputFWD = GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 inputUP = GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);

	//Get the dot product of the forwards and up vector
	float fwdUpDot = glm::dot(inputFWD, inputUP);

	//Calculate the real up vector of the the matrix
	//upAxis = inputUp * (inputFWD - fwdUpDot)
	glm::vec3 realUp = inputUP * (inputFWD - fwdUpDot);
	realUp = glm::length(realUp) != 0.f ? realUp : glm::vec3(0.f, 1.f, 0.f);

	//Get the right vector as the cross product of the 
	//realUp and inputFwd
	glm::vec3 realRight = glm::cross(realUp, inputFWD);
	realRight = glm::length(realRight) != 0.f ? realRight : glm::vec3(1.f, 0.f, 0.f);

	//Set new Real Right and Forward Values
	SetEntityMatrixRow(MATRIX_ROW::UP_VECTOR, realUp); //Set Real UP
	SetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR, realRight);

}