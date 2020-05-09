#ifndef __BRAIN_COMPONENT_H__
#define	__BRAIN_COMPONENT_H__

//C++ Includes
#include <vector>

//GLM Includes
#include <glm/glm.hpp>

//RP3D Inlcudes
#include "ReactPhysics3D/reactphysics3d.h"

//Project Includes
#include "Component.h"

//Forward Declare
class ColliderComponent;
class RayCastHitsInfo;
class Entity;
class Shader;
class DebugUI;
struct UIInputValues;
struct RayCastHit;


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
	glm::vec3 CalculateWanderForce(UIInputValues* a_pUIValues);
	//Flocking Behaviours
	glm::vec3 CalculateFlockingForces(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;
	void ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const;
	//Collision Avoidance
	glm::vec3 CalculateCollisionForces(glm::vec3& a_v3ContainmentForce, glm::vec3& a_v3CollisionAvoidForce) const;
	glm::vec3 CalculateContainmentForce(std::vector<RayCastHit*>& a_vRayCastHits) const;
	glm::vec3 CalculateAvoidanceForce(std::vector<RayCastHit*>& a_vRayCastHits) const;

	//Steering Helper Functions
	inline glm::vec3 GetPointDirection(const glm::vec3& a_v3Start, const glm::vec3& a_v3End) const;

	//Function for getting the current ray directions
	std::vector<rp3d::Ray*> GetCollisionRays() const;
	
	//Current Velocity of the boid
	glm::vec3 m_v3CurrentVelocity;
	
	//Projected point we are wandering to
	glm::vec3 m_v3WanderPoint;

	//Debug UI Instance used to apply weights
	DebugUI* m_pDebugUI;

	#pragma region Boid Defaults
	
	/*
	 * Defaults for the boid, limitiing it's speed, velocity
	 * and force per frame
	 */
	//Max force that can be applied each frame
	const glm::vec3 mc_v3MaxForce = glm::vec3(20.f, 20.f, 20.f);
	const glm::vec3 mc_v3MinForce = glm::vec3(-20.f,-20.f,-20.f);
	//Overall maximum and minimum velocities
	const glm::vec3 mc_v3MaxVelocity = glm::vec3(2.f, 2.f, 2.f);
	const glm::vec3 mc_v3MinVelocity = glm::vec3(-2.f, -2.f, -2.f);

	float m_fNeighbourRadius = 5.0f;
	
	#pragma endregion 

};

#endif // !__BRAIN_COMPONENT_H__
