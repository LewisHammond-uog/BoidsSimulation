#include "Component.h"

//Project Include
#include "Entity.h"

#include <iostream>

Component::Component(Entity* a_pOwner) : m_pOwnerEntity(a_pOwner),
											m_eComponentType(NONE)
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
