#include "Entity.h"

//Typedefs
typedef std::pair<const unsigned int, Entity*> EntityPair;

//Initalise Statics
unsigned int Entity::s_uEntityCount = 0;
std::map<const unsigned int, Entity*> Entity::s_xEntityMap;

Entity::Entity()
{
	//Increment entity count and add to entity list
	m_uEntityID = s_uEntityCount++;
	s_xEntityMap.insert(EntityPair(m_uEntityID, this));
	
}

Entity::~Entity()
{
	//Loop all of the components that this object owns and delete
	//them
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter) {
		delete *xIter;
	}

	//Remove this entity from the entity map and
	//reduce entity count
	//TODO - Check that the entity is in the list?
	s_xEntityMap.erase(m_uEntityID);
	--s_uEntityCount;
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
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent) {
			pComponent->Draw(a_pShader);
		}
	}
}

/// <summary>
/// Adds a component to this entity
/// </summary>
/// <param name="a_pComponentToAdd">Component to Add</param>
void Entity::AddComponent(Component* a_pComponentToAdd)
{
	
	
	//Add component to our component list
	m_apComponentList.push_back(a_pComponentToAdd);
}

/// <summary>
/// Remove a component from this entity 
/// </summary>
/// <param name="a_pComponentToRemove">Component to Remove</param>
void Entity::RemoveComponent(Component* a_pComponentToRemove)
{
	//Loop through all of the components and check to see if we
	//have the component if we do the remove it
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent == a_pComponentToRemove) {
			xIter = m_apComponentList.erase(xIter);
			break;
		}
	}
}
