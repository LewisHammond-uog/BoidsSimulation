#include "BrainComponent.h"

//C++ Includes
#define _USE_MATH_DEFINES //Use Math Defines so we can use pi
#include <math.h>
#include <queue>

//Project Incldues
#include "ColliderComponent.h"
#include "DebugUI.h"
#include "Entity.h"
#include "Gizmos.h"
#include "TransformComponent.h"

BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner),
	m_v3CurrentVelocity(0.0f),
	m_v3WanderPoint(0.0f),
	m_pDebugUI(DebugUI::GetInstance())
{
}

void BrainComponent::Update(float a_fDeltaTime)
{
	//Break if we don't have a UI instance, as we can't
	//control anything
	if(!m_pDebugUI)
	{
		return;
	}

	//Get the UI values
	UIInputValues* pForceValues = m_pDebugUI->GetUIInputValues();
	
	//Update Radius
	m_fNeighbourRadius = pForceValues->fInputNeighbourRadius;
	
	//Get transform component
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Vector for storing all of the forces that we calculate, as we get forces calculated,
	//we add them to the vector in the order that we want to process them (i.e should they be applied?)
	std::queue<glm::vec3> vV3WeightedForces;

	//Get vectors for calculation
	glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();

	/*~~~~COLLISION AVOIDANCE~~~~*/
	//Avoid collision with container and dynamics obsticles
	//Do a raycast so that we can pass it's info to both functions
	glm::vec3 v3ContainmentForce = glm::vec3(0.0f);
	glm::vec3 v3AvoidanceForce = glm::vec3(0.0f);
	ColliderComponent* pCollider = m_pOwnerEntity->GetComponent<ColliderComponent*>();
	if(pCollider)
	{
		//Add forces for containment and collision avoidance - these are very high priority
		RayCastHitsInfo* rayHit = pCollider->RayCast(v3CurrentPos, v3CurrentPos + (v3Forward * m_fNeighbourRadius));
		v3ContainmentForce = CalculateContainmentForce(rayHit) * pForceValues->fInputContainmentForce;
		vV3WeightedForces.push(v3ContainmentForce);
		v3AvoidanceForce = CalculateAvoidanceForce(rayHit) * pForceValues->fInputCollisionAvoidForce;
		vV3WeightedForces.push(v3AvoidanceForce);
		delete rayHit;
	}

	/*~~~~FLOCKING~~~~*/
	//Get all of our flocking values - we call the function by ref so the values are populated
	glm::vec3 v3SeparationForce = glm::vec3(0.f);
	glm::vec3 v3AlignmentForce = glm::vec3(0.f);
	glm::vec3 v3CohesionForce = glm::vec3(0.f);
	CalculateFlockingForces(v3SeparationForce, v3AlignmentForce, v3CohesionForce);
	ApplyFlockingWeights(v3SeparationForce, v3AlignmentForce, v3CohesionForce);
	vV3WeightedForces.push(v3SeparationForce);
	vV3WeightedForces.push(v3AlignmentForce);
	vV3WeightedForces.push(v3CohesionForce);

	/*~~~~WANDER~~~~*/
	//Get and weight wander force
	glm::vec3 v3WanderForce = CalculateWanderForce(pForceValues) * pForceValues->fInputWanderForce;
	vV3WeightedForces.push(v3WanderForce);

	//Do weighted sum calcuations. Apply forces with their weighting and then check if
	//they are over the maximum force
	glm::vec3 v3FinalForce(0.f);
	const int iForceCount = vV3WeightedForces.size();
	for(int i = 0; i < iForceCount; ++i)
	{
		//If we are over our max force then break
		if(glm::length(v3FinalForce) > glm::length(mc_v3MaxForce))
		{
			//if we over the max force then break the loop
			break;
		}

		//Add forces to our final force
		v3FinalForce += vV3WeightedForces.front();
		vV3WeightedForces.pop();
	}
	
	//Clamp values
	v3FinalForce = glm::clamp(v3FinalForce, mc_v3MinForce, mc_v3MaxForce);

	//Calculate Speed and direction, apply limits on speed 
	m_v3CurrentVelocity += v3FinalForce;
	float fSpeed = glm::length(m_v3CurrentVelocity);
	fSpeed = glm::clamp(fSpeed, 2.f, 5.f); //Limit our Speed
	glm::vec3 v3Dir = m_v3CurrentVelocity / fSpeed; //Get our direction, if direction is 0 make it our forward
	m_v3CurrentVelocity = fSpeed * v3Dir; //Re add speed with direction to get new velocity
	
	v3CurrentPos += m_v3CurrentVelocity * a_fDeltaTime;
	v3Forward = glm::length(m_v3CurrentVelocity) > 0.f ? glm::normalize(m_v3CurrentVelocity) : glm::vec3(0.f, 0.f, 1.f);
	 
	//Update our matrix
	pTransform->SetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR, v3Forward);
	pTransform->SetEntityMatrixRow(MATRIX_ROW::POSITION_VECTOR, v3CurrentPos);
	
	//When we update our transform make sure we Orthogonalize the matrix
	pTransform->Orthogonalize();
}

/// <summary>
/// Calculate the force to seek towards a target position
/// </summary>
/// <param name="a_v3Target">Target Position</param>
/// <param name="a_v3CurrentPos">Current Position</param>
/// <returns>Force to seek</returns>
glm::vec3 BrainComponent::CalculateSeekForce(const glm::vec3& a_v3Target, const glm::vec3& a_v3CurrentPos) const
{
	//Calculate Target Direction
	const glm::vec3 v3TargetDir = GetPointDirection(a_v3Target, a_v3CurrentPos);

	//Calc New Velocity
	const glm::vec3 v3NewVelocity = v3TargetDir * mc_fMaxSpeed;

	//Force is target velocity - current velocity
	return (v3NewVelocity - m_v3CurrentVelocity);
}

/// <summary>
/// Calculate the force to flee from a target position
/// </summary>
/// <param name="a_v3Target">Target Position</param>
/// <param name="a_v3CurrentPos">Current Position</param>
/// <returns>Force to flee</returns>
glm::vec3 BrainComponent::CalculateFleeForce(const glm::vec3& a_v3Target, const glm::vec3& a_v3CurrentPos) const
{
	//Calculate Target Direction (away from the )
	const glm::vec3 v3TargetDir = GetPointDirection(a_v3CurrentPos, a_v3Target);

	//Calc New Velocity
	const glm::vec3 v3NewVelocity = v3TargetDir * mc_fMaxSpeed;

	//Force is target velocity - current velocity
	return (v3NewVelocity - m_v3CurrentVelocity);
}

/// <summary>
/// Calculate the wander force by casting a sphere and choosing a point on it
/// </summary>
/// <returns>Force to apply for wander</returns>
glm::vec3 BrainComponent::CalculateWanderForce(UIInputValues* a_pUIValues)
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
	const glm::vec3 v3SphereOrigin = v3CurrentPos + (v3CurrentForward * a_pUIValues->fInputWanderForward);

	//If the magnitude of the vector is 0 then initalize our
	//first wander point
	if (glm::length(m_v3WanderPoint) == 0.0f) 
	{
		//Find a random point omn a sphere
		const glm::vec3 v3RandomPointOnSphere = glm::sphericalRand(a_pUIValues->fInputWanderRadius);
		//Add this point on a sphere to the sphere we are casting out infront of us
		m_v3WanderPoint = v3SphereOrigin + v3RandomPointOnSphere;
	}

	//Calculate direction to move to
	const glm::vec3 v3DirectionToTarget = GetPointDirection(m_v3WanderPoint, v3SphereOrigin) * a_pUIValues->fInputWanderRadius;
	//Find out final target point
	m_v3WanderPoint = v3SphereOrigin + v3DirectionToTarget;
	//Add Jitter
	m_v3WanderPoint += glm::sphericalRand(a_pUIValues->fInputWanderJitter);

	return CalculateSeekForce(m_v3WanderPoint, v3CurrentPos);
}

/// <summary>
/// Gets the direction between 2 points
/// </summary>
/// <param name="a_v3Start">Start Point</param>
/// <param name="a_v3End">End Point</param>
/// <returns>Direction from Start to End Point </returns>
glm::vec3 BrainComponent::GetPointDirection(const glm::vec3& a_v3Start, const glm::vec3& a_v3End) const
{
	//Get vector between 2 points and normalise
	glm::vec3 targetDir = a_v3Start - a_v3End;
	targetDir = glm::length(targetDir) > 0 ? glm::normalize(targetDir) : targetDir;
	return targetDir;
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
		const glm::vec3 v3TargetPos = pTargetTransform->GetCurrentPosition();
		const glm::vec3 v3TargetVelocity = pTargetBrain->GetCurrentVelocity();

		//Get the distance to our target entity and make sure it is within our search radius
		const float fDistanceToTarget = glm::length(v3TargetPos - v3OwnerPos);
		if(fDistanceToTarget < m_fNeighbourRadius)
		{
			/*Calculate the forces in the manner that they should*/
			a_v3SeparationForce += v3OwnerPos - v3TargetPos; //Replusion force
			a_v3AlignmentForce += v3TargetVelocity; //Align our velocity to others
			a_v3CohesionForce += v3TargetPos; //Bring us closer to other boids

			++iNeighbourCount;
		}
	}

	//Our forces should be an average all of the influences we have so we need to
	//divide the current value by the number of influences we had
	if(iNeighbourCount > 0)
	{
		a_v3SeparationForce /= iNeighbourCount;
		a_v3AlignmentForce /= iNeighbourCount;


		a_v3SeparationForce = glm::length(a_v3SeparationForce) != 0 ? glm::normalize(a_v3SeparationForce) : a_v3SeparationForce;
		a_v3AlignmentForce = glm::length(a_v3AlignmentForce) != 0 ? glm::normalize(a_v3AlignmentForce) : a_v3AlignmentForce;
		
		if(glm::length(a_v3CohesionForce))
		{
			a_v3CohesionForce /= iNeighbourCount;
			a_v3CohesionForce = glm::normalize(a_v3CohesionForce - v3OwnerPos);
		}
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
/// <returns></returns>
void BrainComponent::ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const
{
	if(m_pDebugUI == nullptr)
	{
		return;
	}

	//Get the UI values
	UIInputValues* pForceValues = m_pDebugUI->GetUIInputValues();
		
	//Apply the UI weights to the forces
	a_v3SeparationForce *= pForceValues->fInputSeperationForce;
	a_v3AlignmentForce *= pForceValues->fInputAlignmentForce;
	a_v3CohesionForce *= pForceValues->fInputCohesionForce;
}

/// <summary>
/// Calulates the amount of force needed to keep the boid
/// within the containment volume
/// </summary>
/// <returns>(Unweighted) force to turn away from hitting a container</returns>
glm::vec3 BrainComponent::CalculateContainmentForce(RayCastHitsInfo* a_rayResults) const
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
		v3ContainmentForce = containerHit->m_v3HitNormal * (1 - containerHit->m_fHitFraction); 
		v3ContainmentForce = glm::length(v3ContainmentForce) != 0 ? glm::normalize(v3ContainmentForce) : v3ContainmentForce;
	}

	return v3ContainmentForce;
}

/// <summary>
/// Calculate the force needed to avoid any collision with obstacles
/// </summary>
/// <returns>Force to avoid any collision</returns>
glm::vec3 BrainComponent::CalculateAvoidanceForce(RayCastHitsInfo* a_rayResult) const
{
	glm::vec3 v3AvoidForce(0.f);
	//The distance (normalised from our raycast min to max distance)
	//until we hit the obstacle
	float distToCollision = 1.f;

	if(a_rayResult == nullptr)
	{
		return v3AvoidForce;
	}

	RayCastHit* containerHit = nullptr;
	
	//If the detection ray hits a obstacle then we are heading for a collision
	if(!a_rayResult->m_vRayCastHits.empty())
	{
		for (RayCastHit* vRayCastHit : a_rayResult->m_vRayCastHits)
		{
			Entity* hitEntity = vRayCastHit->m_pHitEntity;
			if (hitEntity) {
				ENTITY_TYPE hitType = hitEntity->GetEntityType();
				if (hitType == ENTITY_TYPE::ENTITY_TYPE_BOID || hitType == ENTITY_TYPE::ENTITY_TYPE_OBSTACLE)
				{
					containerHit = vRayCastHit;
					break;
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
		v3AvoidForce = containerHit->m_v3HitNormal * (containerHit->m_fHitFraction);
		v3AvoidForce = glm::length(v3AvoidForce) != 0 ? glm::normalize(v3AvoidForce) : v3AvoidForce;
	}
	

	return v3AvoidForce;
}
