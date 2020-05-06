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

//todo reorginise
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

	//Steering Helper Functions
	inline glm::vec3 GetPointDirection(const glm::vec3& a_v3Start, const glm::vec3& a_v3End) const;

	//Flocking Behaviours
	glm::vec3 CalculateFlockingForces(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;
	void ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;

	//Collision Avoidance
	glm::vec3 CalculateContainmentForce(RaycastCallbackInfo* a_rayResults) const;
	glm::vec3 CalculateAvoidanceForce(RaycastCallbackInfo* a_collisionDetectRay, ColliderComponent* a_pRaycaster, glm::vec3 a_v3CastPos);
	glm::vec3 GetCollisionAvoidDirection(ColliderComponent* a_pRaycaster, glm::vec3 a_v3CastPos);
	//Collision Avoidance Settings
	static const int sc_iCollisionAvoidanceRayCount = 200; //Number of directions to look in when avoiding collisions
	const float mc_fLookAheadDist = 1.0f;

	//Function used to precompute directions to look in when doing collision checks
	void ComputeCollisionDirections();
	static glm::vec3 s_aCollisionDirections[];
	static bool s_bCollisionDirectionsInit;

	glm::vec3 m_v3PrevSafeDir = glm::vec3(0);
	
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
	const glm::vec3 mc_v3MaxVelocity = glm::vec3(5.f, 5.f, 5.f);
	const glm::vec3 mc_v3MinVelocity = glm::vec3(-5.f, -5.f, -5.f);
	const float mc_fNeighbourhoodRadius = 5.0f;//todo prehaps default?

	//Wander Defaults
	const float mc_fSphereForwardMutiplier = 1.f; //How far forward to draw the sphere
	const float mc_fWanderJitter = 0.5f; //How much to move from the point on the spehre
	const float mc_fWanderRadius = 4.0f; //How large the sphere is
	#pragma endregion 

};

#endif // !__BRAIN_COMPONENT_H__
