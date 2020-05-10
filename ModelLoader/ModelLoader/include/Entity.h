#ifndef __ENTITY_H__
#define __ENTITY_H__

//std includes
#include <vector>
#include <map>

//Project Includes
#include <string>

#include "Component.h"

//Forward Declare
class Shader;

//Enum for Types of entity
enum class ENTITY_TYPE
{
	ENTITY_TYPE_UNDEFINED,
	ENTITY_TYPE_BOID,
	ENTITY_TYPE_CONTAINER,
	ENTITY_TYPE_OBSTACLE,
	ENTITY_TYPE_CAMERA,

	ENTITY_TYPE_COUNT //Total number of entity types
};

/// <summary>
/// Class to represent any object that exists within the world
/// Has a number of components that are updated by this entity
/// </summary>
class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	//Entity Type Functions
	void SetEntityType(ENTITY_TYPE a_eType);
	ENTITY_TYPE GetEntityType() const;
	const char* GetEntityTypeName() const;

	//Component Functions
	void AddComponent(Component* a_pComponentToAdd);
	void RemoveComponent(Component* a_pComponentToRemove, bool a_bDeleteComponent = false);
	std::vector<Component*> GetComponentList();
	
	//Function for getting a component of this entity based on it's return type
	template<class returnType>
	returnType GetComponent() const;
	
	unsigned int GetEntityID() const { return m_uEntityID; }
	static const std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityMap; }

private:

	ENTITY_TYPE m_eEntityType;
	
	unsigned int m_uEntityID;
	std::vector<Component*> m_apComponentList;

	static unsigned int s_uNextEntityID;
	static std::map<const unsigned int, Entity*> s_xEntityMap;
};

/// <summary>
/// Template Function that returns a pointer to a component, based on the
/// template argument (returnType), for example GetComponent<TransformComponent*>() will return
/// a pointer to the transform component of the calling entity.
/// Template Type must be a a Component* (e.g TransformComponent*, ModelComponent*)
/// Will return nullptr if there is not a component of returnType attached to this entity 
/// </summary>
/// <returns>Pointer to the component of the returnType attached to this entity or
/// nullptr if one is not attached</returns>
template<class returnType>
returnType Entity::GetComponent() const
{
	//Loop through all of the components that we have,
	//try and cast each one to the returnType, if we can cast to it
	//then it is a component of the type that we are looking to return, so return it.
	//If we go through all of the components that we have and have not succesfully
	//cast then we do not have a component of returnType, so return nullptr.
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		returnType pComponent = dynamic_cast<returnType>(*xIter);
		if (pComponent != nullptr)
		{
			return pComponent;
		}
	}

	return nullptr;
}

#endif // ! __ENTITY_H__
