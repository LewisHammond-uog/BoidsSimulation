#include "BrainComponent.h"

//Project Incldues
#include "ColliderComponent.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "DebugUI.h"
#include "Gizmos.h"

//TODO Clean UP/Remove
//Constants
const float fMAX_SPEED = 5.0f;
const float fMAX_FORCE = 0.2f;
const float fNEIGHBOURHOOD_RADIUS = 5.0f;

const float fCIRCLE_FORWARD_MUTIPLIER = 1.f; //How far forward to draw the sphere
const float fJITTER = 0.5f; //How much to move from the point on the spehre
const float fWANDER_RADIUS = 4.0f; //How large the sphere is

BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner),
	m_pDebugUI(DebugUI::GetInstance()),
	m_v3CurrentVelocity(0.0f),
	m_v3WanderPoint(0.0f)
{

}

void BrainComponent::Update(float a_fDeltaTime)
{	
	//Get transform component
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Get vectors for calculation
	glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();

	/*~~~~COLLISION AVOIDANCE~~~~*/
	//Do a raycast so that we can pass it's info to both functions
	glm::vec3 v3ContainmentForce = glm::vec3(0.0f);
	ColliderComponent* pCollider = m_pOwnerEntity->GetComponent<ColliderComponent*>();
	if(pCollider)
	{
		RaycastCallbackInfo rayHit = pCollider->RayCast(v3CurrentPos, v3CurrentPos + v3Forward);
		v3ContainmentForce = CalculateContainmentForce(&rayHit);
	}

	/*~~~~FLOCKING~~~~*/
	//Get all of our flocking values - we call the function by ref so the values are populated
	glm::vec3 v3SeparationForce, v3AlignmentForce, v3CohesionForce = glm::vec3(0.f);
	CalculateFlockingForces(v3SeparationForce, v3AlignmentForce, v3CohesionForce);
	ApplyFlockingWeights(v3SeparationForce, v3AlignmentForce, v3CohesionForce);

	/*~~~~WANDER~~~~*/
	//Get and weight wander force
	glm::vec3 v3WanderForce = CalculateWanderForce() * m_pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_WANDER);
	
	//Apply Force
	m_v3CurrentVelocity += v3WanderForce + v3ContainmentForce;
	//Apply Velocity to Position
	v3CurrentPos += m_v3CurrentVelocity * a_fDeltaTime;
	v3Forward = glm::length(m_v3CurrentVelocity) > 0.f ? glm::normalize(m_v3CurrentVelocity) : glm::vec3(0.f, 0.f, 1.f);
	 
	//Update our matrix
	pTransform->SetEntityMatrixRow(FORWARD_VECTOR, v3Forward);
	pTransform->SetEntityMatrixRow(POSTION_VECTOR, v3CurrentPos);
	//When we update our transform make sure we Orthogonalize the matrix
	pTransform->Orthogonalize();
}



//TODO Restructure so that we don't repeat ourselves - CalculateFleeForce
glm::vec3 BrainComponent::CalculateSeekForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const
{
	//Calculate Target Direction
	glm::vec3 v3TargetDir = v3Target - v3CurrentPos;

	//only normalise if not a zero vector
	if (glm::length(v3TargetDir) > 0.0f) 
	{
		v3TargetDir = glm::normalize(v3TargetDir);
	}

	//Calc New Velocity
	const glm::vec3 v3NewVelocity = v3TargetDir * fMAX_SPEED;

	//Force is target velocity - current velocity
	return (v3NewVelocity - m_v3CurrentVelocity);
}

//TODO Restructure so that we don't repeat ourselves - CalculateSeekForce
glm::vec3 BrainComponent::CalculateFleeForce(const glm::vec3& v3Target, const glm::vec3& v3CurrentPos) const
{
	//Calculate Target Direction (away from the )
	glm::vec3 v3TargetDir = v3CurrentPos - v3Target;

	//only normalise if not a zero vector
	if (glm::length(v3TargetDir) > 0.0f)
	{
		v3TargetDir = glm::normalize(v3TargetDir);
	}

	//Calc New Velocity
	const glm::vec3 v3NewVelocity = v3TargetDir * fMAX_SPEED;

	//Force is target velocity - current velocity
	return (v3NewVelocity - m_v3CurrentVelocity);
}

/// <summary>
/// Calculate the wander force by casting a sphere and choosing a point on it
/// </summary>
/// <returns>Force to apply for wander</returns>
glm::vec3 BrainComponent::CalculateWanderForce()
{
	//Check that parent is not null
	if(m_pOwnerEntity == nullptr)
	{
		return glm::vec3(0.f);
	}
	
	//Get our current position and forward
	TransformComponent* pParentTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if(pParentTransform == nullptr)
	{
		return glm::vec3(0.f);
	}
	const glm::vec3 v3CurrentPos = pParentTransform->GetCurrentPosition();
	const glm::vec3 v3CurrentForward = pParentTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	
	//Project a point in front of us for the center of our sphere
	const glm::vec3 v3SphereOrigin = v3CurrentPos + (v3CurrentForward * fCIRCLE_FORWARD_MUTIPLIER);

	//If the magnitude of the vector is 0 then initalize our
	//first wander point
	if (glm::length(m_v3WanderPoint) == 0.0f) 
	{
		//Find a random point omn a sphere
		const glm::vec3 v3RandomPointOnSphere = glm::sphericalRand(fWANDER_RADIUS);

		//Add this point on a sphere to the sphere we are casting out infront of us
		m_v3WanderPoint = v3SphereOrigin + v3RandomPointOnSphere;
	}

	//Calculate direction to move to
	const glm::vec3 v3DirectionToTarget = glm::normalize(m_v3WanderPoint - v3SphereOrigin) * fWANDER_RADIUS;

	//Find out final target point
	m_v3WanderPoint = v3SphereOrigin + v3DirectionToTarget;

	//Add Jitter
	m_v3WanderPoint += glm::sphericalRand(fJITTER);

	return CalculateSeekForce(m_v3WanderPoint, v3CurrentPos);
}



/// <summary>
/// Calculates all of the flocking forces
/// Does not take in to account any weighting of values
/// from the UI
/// </summary>
/// <param name="a_v3SeparationForce">ByRef Separation Force to fill with value</param>
/// <param name="a_v3AlignmentForce">ByRef Alignment Force to fill with value</param>
/// <param name="a_v3CohesionForce">ByRef Cohesion Force to fill with value</param>
/// <returns>Total (unweighted) force</returns>
glm::vec3 BrainComponent::CalculateFlockingForces(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const
{
	//Store the number of neighbours we are interacted with - so we can avg. forces
	int iNeighbourCount = 0;
	
	//Null Check our owner
	if (m_pOwnerEntity == nullptr)
	{
		return glm::vec3(0.f);
	}

	//Get our transform
	TransformComponent* pOwnerTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if(pOwnerTransform == nullptr)
	{
		return glm::vec3(0.f);
	}
	//Get our position it is the only part of the transform we use
	glm::vec3 v3OwnerPos = pOwnerTransform->GetCurrentPosition();

	/*
	Loop through all of our entities and calculate
	Separation, Alignment, Cohesion forces
	*/
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Get the current target entity, make sure it is valid
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}
		//Check that our target is not this entity
		if(pTarget == m_pOwnerEntity)
		{
			continue;
		}
		
		//Get the targets transform and check it is valid
		TransformComponent* pTargetTransform = pTarget->GetComponent<TransformComponent*>();
		//Get the targets brain - used for alignment force
		BrainComponent* pTargetBrain = pTarget->GetComponent<BrainComponent*>();

		//Null Check the components we just got
		if(!pTargetTransform || !pTargetBrain)
		{
			continue;
		}

		//Get the values we want from the components we just go
		glm::vec3 v3TargetPos = pTargetTransform->GetCurrentPosition();
		glm::vec3 v3TargetVelocity = pTargetBrain->GetCurrentVelocity();

		//Get the distance to our target entity and make sure it is within our search radius
		float fDistanceToTarget = glm::length(v3TargetPos - v3OwnerPos);
		if(fDistanceToTarget < fNEIGHBOURHOOD_RADIUS)
		{
			/*Calculate the forces in the manner that they should*/
			a_v3SeparationForce += v3OwnerPos - v3TargetPos; //Replusion force
			a_v3AlignmentForce += v3TargetVelocity; //Align our velocity to others
			a_v3CohesionForce += v3TargetPos; //Bring us closer to other boids

			++iNeighbourCount;
		}
	}

	//todo seperate function?
	//Our forces should be an average all of the influences we have so we need to
	//divide the current value by the number of influences we had
	if(iNeighbourCount > 0)
	{
		a_v3SeparationForce /= iNeighbourCount;
		a_v3AlignmentForce /= iNeighbourCount;
		a_v3CohesionForce /= iNeighbourCount;

		a_v3SeparationForce = glm::length(a_v3SeparationForce) != 0 ? glm::normalize(a_v3SeparationForce) : a_v3SeparationForce;
		a_v3AlignmentForce = glm::length(a_v3AlignmentForce) != 0 ? glm::normalize(a_v3AlignmentForce) : a_v3AlignmentForce;
		a_v3CohesionForce = glm::length(a_v3CohesionForce) != 0 ? glm::normalize(a_v3CohesionForce) : a_v3CohesionForce;
	}

	//Return the final total force
	return a_v3SeparationForce + a_v3AlignmentForce + a_v3CohesionForce;
}

/// <summary>
/// Apply Flocking weights (that we get from the debug UI) to the given force values
/// </summary>
/// <param name="a_v3SeparationForce">ByRef Separation Force to modify</param>
/// <param name="a_v3AlignmentForce">ByRef Alignment Force to modify</param>
/// <param name="a_v3CohesionForce">ByRef Cohesion Force to modify</param>
/// <param name="a_pDebugUI">Debug UI instance to use to get weights</param>
/// <returns></returns>
void BrainComponent::ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const
{
	if(m_pDebugUI == nullptr)
	{
		return;
	}
		
	//Apply the UI weights to the forces
	a_v3SeparationForce *= m_pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_SEPERATION);
	a_v3AlignmentForce *= m_pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_ALIGNMENT);
	a_v3CohesionForce *= m_pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_COHESION);
}

/// <summary>
/// Calulates the amount of force needed to keep the boid
/// within the containment volume
/// </summary>
/// <returns>(Unweighted) force to turn away from hitting a container</returns>
glm::vec3 BrainComponent::CalculateContainmentForce(RaycastCallbackInfo* a_rayResults) const
{
	//Store our containment force - init to 0 so we can return this var if we don't hit
	glm::vec3 v3ContainmentForce(0.0f);
	
	//Work out if we hit a containtment object, point to it if we have so
	//we can get more infomation
	RayCastHit* containerHit = nullptr;
	if(!a_rayResults->m_vRayCastHits.empty())
	{
		for (RayCastHit* m_vRayCastHit : a_rayResults->m_vRayCastHits)
		{
			Entity* hitEntity = m_vRayCastHit->m_pHitEntity;
			if (hitEntity) {
				if (hitEntity->GetEntityType() == ENTITY_TYPE::ENTITY_TYPE_CONTAINER)
				{
					containerHit = m_vRayCastHit;
				}
			}
		}
	}
	
	//If we have hit a container then calculate our force otherwise we will just return 0
	if (containerHit != nullptr)
	{
		//Get the normal and return our force in that direction so we turn away from the object,
		//mutiply it by the distance to the wall, so our force gets more agressive the closer we get
		//Invert the m_fHitFraction because 1 means it is a the very end of the ray, we want the opposite multiplication
		v3ContainmentForce = containerHit->m_v3HitNormal * (1.f - containerHit->m_fHitFraction); 
		v3ContainmentForce = glm::length(v3ContainmentForce) != 0 ? glm::normalize(v3ContainmentForce) : v3ContainmentForce;
	}

	return v3ContainmentForce;
}
