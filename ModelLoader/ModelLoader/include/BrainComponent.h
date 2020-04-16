#ifndef __BRAIN_COMPONENT_H__
#define	__BRAIN_COMPONENT_H__

#include "Component.h"

#include <glm/glm.hpp>

//Forward Declate
class Entity;
class Shader;

class BrainComponent : public Component
{
public:
	BrainComponent(Entity* a_pOwner);

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader) {};

	glm::vec3 GetCurrentVelocity() const { return m_v3CurrentVelocity; }

private:
	
	//Steering Behaviours
	glm::vec3 CalculateSeekForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateFleeForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateWanderForce(const glm::vec3& v3Forward, const glm::vec3& v3CurrentPos);

	//Flocking Behaviours
	glm::vec3 CalculateSeperationForce();
	glm::vec3 CalculateAlignmentForce();
	glm::vec3 CalculateCohensionForce();

	glm::vec3 m_v3CurrentVelocity; 
	glm::vec3 m_v3WanderPoint;

};

#endif // !__BRAIN_COMPONENT_H__
