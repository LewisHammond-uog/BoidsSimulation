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
