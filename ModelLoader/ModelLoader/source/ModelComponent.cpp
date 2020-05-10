#include "ModelComponent.h"

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"

//Lib Includes
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <glm/gtc/matrix_transform.hpp>

#include "MathsUtils.h"

//Typedefs
typedef Component PARENT;

/// <summary>
/// Create a model coponent
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
ModelComponent::ModelComponent(Entity* a_pOwner): 
	PARENT(a_pOwner),
	m_pModelData(nullptr),
	m_fModelScale(0.0f)																
{
}

/// <summary>
/// Draw the model we have been provided
/// </summary>
/// <param name="a_pShader">Shader to draw model with</param>
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
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	
	glm::mat4 modelMatrix = pTransform->GetEntityMatrix();
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_fModelScale, m_fModelScale, m_fModelScale));
	a_pShader->setMat4("model", modelMatrix);
	m_pModelData->Draw(*a_pShader);
}

/// <summary>
/// Chooses a random model for a given list
/// </summary>
void ModelComponent::ChooseRandomModel(std::vector<Model*>& a_vpModels)
{
	if (!a_vpModels.empty()) {
		//Select a Random Model from our list
		const int chosenIndex = MathsUtils::RandomRange<int>(0, a_vpModels.size());
		//Lets Choose A Model
		Model* selectedModel = a_vpModels[chosenIndex];

		if (selectedModel != nullptr)
		{
			SetModel(selectedModel);
		}
	}
}

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns></returns>
const char* ModelComponent::GetComponentName() const
{
	return m_szName;
}