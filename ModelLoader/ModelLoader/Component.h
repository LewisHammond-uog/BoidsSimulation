#ifndef __COMPONENT_H__
#define __COMPONENT_H__

//Forward Declare
class Entity;
class Shader;


enum COMPONENT_TYPE {
	NONE,
	TRANSFORM,
	MODEL,
	BRAIN,
};

class Component
{
public:
	Component(Entity* a_pOwner);
	~Component();

	//To do - remove?
	//[TO DO] - Shared Function for Update/Draw?
	virtual void Update(float a_fDeltaTime) = 0; //Pure Virtual Function
	virtual void Draw(Shader* a_pShader) = 0; //Pure Virtual Function

	Entity* GetOwnerEntity() { return m_pOwnerEntity; }
	inline COMPONENT_TYPE GetComponentType() { return m_eComponentType; }

protected:
	Entity* m_pOwnerEntity;
	COMPONENT_TYPE m_eComponentType;
};

#endif	//!__COMPONENT_H__

