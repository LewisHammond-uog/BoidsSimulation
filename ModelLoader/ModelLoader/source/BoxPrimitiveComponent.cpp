#include "BoxPrimitiveComponent.h"

//Gizmos Include
#include "Gizmos.h"

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"

//Typedefs
typedef PrimitiveComponent PARENT;

/// <summary>
/// Create a box primitive
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
/// <param name="a_v3BoxDimensions">Dimentions of the box</param>
BoxPrimitiveComponent::BoxPrimitiveComponent(Entity* a_pOwner, const glm::vec3 a_v3BoxDimensions/* = glm::vec3(1,1,1)*/) :
	PARENT(a_pOwner),
	m_v3BoxDimensions(a_v3BoxDimensions)
{

	//Set our Primitive type
	m_ePrimitiveType = PrimitiveTypes::BOX;

}

/// <summary>
/// Sets the the Dimensions of the box
/// </summary>
/// <param name="a_v3NewDimensions">New dimensions of the box</param>
void BoxPrimitiveComponent::SetDimensions(const glm::vec3 a_v3NewDimensions)
{
	m_v3BoxDimensions = a_v3NewDimensions;
}

/// <summary>
/// Draw the box primitive component
/// </summary>
/// <param name="a_pShader"></param>
void BoxPrimitiveComponent::Draw(Shader* a_pShader)
{
	//Get the transform component - so that we can create the box at our position
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Draw a box to the sphere at our current position, with the given dimentions, in the specified colour, and have the trasform of our owner entity
	Gizmos::addBox(pTransform->GetCurrentPosition(), m_v3BoxDimensions, true, m_v4DrawColour, pTransform->GetEntityMatrix());
}

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns></returns>
const char* BoxPrimitiveComponent::GetComponentName() const
{
	return m_szName;
}
