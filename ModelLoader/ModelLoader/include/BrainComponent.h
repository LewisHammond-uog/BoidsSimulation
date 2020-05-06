#ifndef __BRAIN_COMPONENT_H__
#define	__BRAIN_COMPONENT_H__

#include "Component.h"

#include <glm/glm.hpp>

class ColliderComponent;
class RaycastCallbackInfo;
//Forward Declare
class Entity;
class Shader;
class DebugUI;


class BrainComponent : public Component
{
public:
	BrainComponent(Entity* a_pOwner);
	~BrainComponent() = default;

	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override {};

	glm::vec3 GetCurrentVelocity() const { return m_v3CurrentVelocity; }

private:
	
	//Steering Behaviours
	glm::vec3 CalculateSeekForce(const glm::vec3& a_v3Target, const glm::vec3& a_v3CurrentPos) const;
	glm::vec3 CalculateFleeForce(const glm::vec3& a_v3Target, const glm::vec3& a_v3CurrentPos) const;
	glm::vec3 CalculateWanderForce();

	//Flocking Behaviours
	glm::vec3 CalculateFlockingForces(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;
	void ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;

	//Collision Avoidance
	glm::vec3 CalculateContainmentForce(RaycastCallbackInfo* a_rayResults) const;
	glm::vec3 CalculateAvoidanceForce(RaycastCallbackInfo* a_rayResults) const;

	//Steering Helper Functions
	inline glm::vec3 GetPointDirection(const glm::vec3& a_v3Start, const glm::vec3& a_v3End) const;


	//Current Velocity of the boid
	glm::vec3 m_v3CurrentVelocity;
	//Projected point we are wandering to
	glm::vec3 m_v3WanderPoint;

	//Debug UI Instance used to apply weights
	DebugUI* m_pDebugUI;

	#pragma region Boid Defaults
	
	//Brain Defaults - Max force and our neighbour hood
	const float mc_fMaxSpeed = 0.2f;
	const glm::vec3 mc_v3MaxForce = glm::vec3(0.2f,0.2f,0.2f);
	const glm::vec3 mc_v3MinForce = glm::vec3(-0.2f,-0.2f,-0.2f);
	const glm::vec3 mc_v3MaxVelocity = glm::vec3(2.f, 2.f, 2.f);
	const glm::vec3 mc_v3MinVelocity = glm::vec3(-2.f, -2.f, -2.f);
	
	float m_fNeighbourRadius = 5.0f;
	
	#pragma endregion 

};

#endif // !__BRAIN_COMPONENT_H__
