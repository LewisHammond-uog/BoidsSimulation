#include "ModelComponent.h"

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"

//Lib Includes
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <glm/gtc/matrix_transform.hpp>

//Typedefs
typedef Component PARENT;

//todo remove
#include "BrainComponent.h"
#include "BoxPrimitiveComponent.h"

ModelComponent::ModelComponent(Entity* a_pOwner): 
	PARENT(a_pOwner),
	m_pModelData(nullptr),
	m_fModelScale(0.0f)																
{
}

void ModelComponent::Draw(Shader* a_pShader)
{
	//Null Check shader
	if (!a_pShader) {
		return;
	}

	//Check for model data
	if (!m_pModelData) {
		return;
	}

	//Get the transform component
	TransformComponent* pTransform = static_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(TRANSFORM));
	if (!pTransform) {
		return;
	}

	TransformComponent* test = m_pOwnerEntity->GetComponentT<TransformComponent*>();
	BrainComponent* brain = m_pOwnerEntity->GetComponentT<BrainComponent*>();
	BoxPrimitiveComponent* nulllllll = m_pOwnerEntity->GetComponentT<BoxPrimitiveComponent*>();
	
	glm::mat4 modelMatrix = pTransform->GetEntityMatrix();
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_fModelScale, m_fModelScale, m_fModelScale));
	a_pShader->setMat4("model", modelMatrix);
	m_pModelData->Draw(*a_pShader);
}
