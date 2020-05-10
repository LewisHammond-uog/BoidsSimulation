#include "SpherePrimitiveComponent.h"



#include "Entity.h"
#include "Gizmos.h"
#include "TransformComponent.h"

//Typedefs
typedef PrimitiveComponent PARENT;

/// <summary>
/// Create a sphere primative
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
/// <param name="a_fSphereRadius">Radius of the Sphere</param>
SpherePrimitiveComponent::SpherePrimitiveComponent(Entity* a_pOwner, const float a_fSphereRadius) :
	PARENT(a_pOwner),
	m_fSphereRadius(a_fSphereRadius)
{

	//Set our Primitive type
	m_ePrimitiveType = PrimitiveTypes::PRIMITIVE_TYPE_SPHERE;
}

/// <summary>
/// Set the radius of the sphere
/// </summary>
/// <param name="a_fNewRadius">Radius to the Sphere</param>
void SpherePrimitiveComponent::SetDimensions(const float a_fNewRadius)
{
	m_fSphereRadius = a_fNewRadius;
}

void SpherePrimitiveComponent::Draw(Shader* a_pShader)
{
	//Get the transform component - so that we can create the box at our position
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Draw a sphere to the screen, centered around the current position of the owner enity, at the given resolution, in the given colour,
	//with our owners transform
	Gizmos::addSphere(pTransform->GetCurrentPosition(), m_iSphereResolution, m_iSphereResolution, m_fSphereRadius,
		m_v4DrawColour, &pTransform->GetEntityMatrix());
}

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns></returns>
const char* SpherePrimitiveComponent::GetComponentName() const
{
	return m_szName;
}