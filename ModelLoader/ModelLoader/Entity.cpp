#include "Entity.h"

//Typedefs
typedef std::pair<const unsigned int, Entity*> EntityPair;

//Initalise Statics
unsigned int Entity::s_uEntityCount = 0;
std::map<const unsigned int, Entity*> Entity::s_xEntityList;

Entity::Entity()
{
	//Increment entity count and add to entity list
	m_uEntityID = s_uEntityCount++;
	s_xEntityList.insert(EntityPair(m_uEntityID, this));
	
}

Entity::~Entity()
{
}

///Update this entity
void Entity::Update(float a_fDeltaTime)
{
	//Loop through all of the components and update
	std::vector<Component*>::iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter) 
	{
		Component* pComponent = *xIter;
		if (pComponent) {
			pComponent->Update(a_fDeltaTime);
		}
	}
}

///Draw all elements of this entity
void Entity::Draw(Shader* a_pShader)
{
	//Loop through all of the components and draw
	std::vector<Component*>::iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent) {
			pComponent->Draw(a_pShader);
		}
	}
}

///Add a component to this entity
void Entity::AddComponent(Component* a_pComponent)
{
	//TO DO DUPLICATE CHECKING
	m_apComponentList.push_back(a_pComponent);
}

Component* Entity::GetComponent(COMPONENT_TYPE a_eComponentType) const
{
	//Loop through all of the components see if they have a component
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent && pComponent->GetComponentType() == a_eComponentType) {
			return pComponent;
		}
	}

	//No component found, nullptr
	return nullptr;

}
