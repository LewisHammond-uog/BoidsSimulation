#include "Component.h"

//Project Include
#include "Entity.h"

Component::Component(Entity* a_pOwner) : m_pOwnerEntity(a_pOwner),
											m_eComponentType(NONE)
{
}


Component::~Component()
{
}
