#include "Component.h"

//Project Include
#include "Entity.h"

Component::Component(Entity* a_pOwner) : m_pOwnerEntity(a_pOwner)
{
}


Component::~Component()
{
	//When we are destroyed remove ourselves from our
	//owners components
	if(m_pOwnerEntity)
	{
		m_pOwnerEntity->RemoveComponent(this);
	}
}

/// <summary>
/// Gets the entity that owns this component
/// </summary>
/// <returns>Owner Entity</returns>
Entity* Component::GetOwnerEntity() const
{
	return m_pOwnerEntity;
}

/// <summary>
/// Remove the owner entity from this component
/// </summary>
void Component::RemoveOwnerEntity()
{
	m_pOwnerEntity = nullptr;
}