#ifndef __COMPONENT_H__
#define __COMPONENT_H__

//Forward Declare
class Entity;
class Shader;



class Component
{
public:
	Component(Entity* a_pOwner);
	~Component();

	//todo - Shared Function for Update/Draw?
	virtual void Update(float a_fDeltaTime) = 0; //Pure Virtual Function
	virtual void Draw(Shader* a_pShader) = 0; //Pure Virtual Function

	Entity* GetOwnerEntity() const;
	void RemoveOwnerEntity();


protected:
	Entity* m_pOwnerEntity;
};

#endif	//!__COMPONENT_H__

