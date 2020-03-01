#ifndef __ENTITY_H__
#define __ENTITY_H__

//std includes
#include <vector>
#include <map>

//project includes
#include "Component.h"

//Forward Declare
class Shader;


class Entity
{
public:
	Entity();
	~Entity();

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_fDeltaTime);

	void AddComponent(Component* a_pComponent);
	Component* GetComponent(COMPONENT_TYPE a_eComponentType) const;

	unsigned int GetEntityID() const { return m_uEntityID; }

	static const std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityMap; }

private:
	unsigned int m_uEntityID;
	std::vector<Component*> m_apComponentList;


	static unsigned int s_uEntityCount;
	static std::map<const unsigned int, Entity*> s_xEntityMap;
};

#endif // ! __ENTITY_H__