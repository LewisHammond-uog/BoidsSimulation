#include "TransformComponent.h"

//Typedefs
typedef Component PARENT;

TransformComponent::TransformComponent(Entity* a_pOwner) : 
	PARENT(a_pOwner),
	m_m4EntityMatrix(glm::mat4(1.0f))
{
	m_eComponentType = TRANSFORM;
}

void TransformComponent::SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec)
{
	float finalVec4Value = a_eRow == POSTION_VECTOR ? 1.f : 0.0f;
	m_m4EntityMatrix[a_eRow] = glm::vec4(a_v3Vec, finalVec4Value);
}

glm::vec3 TransformComponent::GetEntityMatrixRow(MATRIX_ROW a_eRow)
{
	return m_m4EntityMatrix[a_eRow];
}
