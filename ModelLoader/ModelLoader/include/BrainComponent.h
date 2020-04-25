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

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader) {};

	glm::vec3 GetCurrentVelocity() const { return m_v3CurrentVelocity; }

private:
	
	//Steering Behaviours
	glm::vec3 CalculateSeekForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateFleeForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const;
	glm::vec3 CalculateWanderForce();

	//Flocking Behaviours
	glm::vec3 CalculateFlockingForces(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;
	void ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;

	//Collision Avoidance
	glm::vec3 CalculateContainmentForce(RaycastCallbackInfo* a_rayResults) const;
	glm::vec3 CalculateAvoidanceForce(RaycastCallbackInfo* a_collisionDetectRay, ColliderComponent* a_pRaycaster, glm::vec3 a_v3CastPos) const;
	glm::vec3 GetCollisionAvoidDirection(ColliderComponent* a_pRaycaster, glm::vec3 a_v3CastPos) const;
	//Collision Avoidance Settings
	static const int sc_iCollisionAvoidanceRayCount = 200; //Number of directions to look in when avoiding collisions
	const float mc_fLookAheadDist = 1.0f;

	//Function used to precompute directions to look in when doing collision checks
	void ComputeCollisionDirections();
	static glm::vec3 s_aCollisionDirections[];
	static bool s_bCollisionDirectionsInit;
	
	//Debug UI Instance used to apply weights
	DebugUI* m_pDebugUI;

	glm::vec3 m_v3CurrentVelocity; 
	glm::vec3 m_v3WanderPoint;

};

#endif // !__BRAIN_COMPONENT_H__
