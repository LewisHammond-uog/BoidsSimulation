#ifndef __ENTITY_H__
#define __ENTITY_H__

//std includes
#include <vector>
#include <map>

//Project Includes
#include "Component.h"
#include "TransformComponent.h"

//Forward Declare
class Shader;

class Entity
{
public:
	Entity();
	~Entity();

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	void AddComponent(Component* a_pComponent);
	Component* GetComponent(COMPONENT_TYPE a_eComponentType) const;

	//Function for getting a component of this entity based on it's return type
	template<class returnType>
	returnType GetComponentT();

	unsigned int GetEntityID() const { return m_uEntityID; }

	static const std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityMap; }

private:
	unsigned int m_uEntityID;
	std::vector<Component*> m_apComponentList;


	static unsigned int s_uEntityCount;
	static std::map<const unsigned int, Entity*> s_xEntityMap;
};

/// <summary>
/// Template Function that returns a pointer to a component, based on the
/// template arguments, for example GetComponent<TransformComponent>() will return
/// a pointer to the transform component of the calling entity
/// Will return nullptr 
/// </summary>
/// <returns></returns>
template<class returnType>
returnType Entity::GetComponentT()
{
	//Loop through all of our components and if we find the type
	//we are looking for then return it, otherwise return a nullptr
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
