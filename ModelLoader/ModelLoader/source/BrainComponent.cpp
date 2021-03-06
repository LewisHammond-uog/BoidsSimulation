#include "BrainComponent.h"

//C++ Includes
#include <queue>


//Project Incldues
#include "ColliderComponent.h"
#include "RaycastComponent.h"
#include "DebugUI.h"
#include "Entity.h"
#include "Gizmos.h"
#include "TransformComponent.h"

/// <summary>
/// Create the brain component
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner),
	m_v3CurrentVelocity(0.0f),
	m_v3WanderPoint(0.0f),
	m_pDebugUI(DebugUI::GetInstance())
{
}

/// <summary>
/// Update the brain component. Calcuating new forces for flocking, steering and
/// collision avoidance
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
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
	m_fNeighbourRadius = pForceValues->fInputNeighbourRadius.value;
	
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
	CalculateCollisionForces(v3ContainmentForce, v3AvoidanceForce);
	vV3WeightedForces.push(v3ContainmentForce * m_pDebugUI->GetUIInputValues()->fInputContainmentForce.value);
	vV3WeightedForces.push(v3AvoidanceForce * m_pDebugUI->GetUIInputValues()->fInputContainmentForce.value);

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
	glm::vec3 v3WanderForce = CalculateWanderForce(pForceValues) * pForceValues->fInputWanderForce.value;
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
	m_v3CurrentVelocity = glm::clamp(m_v3CurrentVelocity, mc_v3MinVelocity, mc_v3MaxVelocity);
	
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
	const glm::vec3 v3NewVelocity = v3TargetDir;

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
	const glm::vec3 v3NewVelocity = v3TargetDir;

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
	const glm::vec3 v3SphereOrigin = v3CurrentPos + (v3CurrentForward * a_pUIValues->fInputWanderForward.value);

	//If the magnitude of the vector is 0 then initalize our
	//first wander point
	if (glm::length(m_v3WanderPoint) == 0.0f) 
	{
		//Find a random point omn a sphere
		const glm::vec3 v3RandomPointOnSphere = glm::sphericalRand(a_pUIValues->fInputWanderRadius.value);
		//Add this point on a sphere to the sphere we are casting out infront of us
		m_v3WanderPoint = v3SphereOrigin + v3RandomPointOnSphere;
	}

	//Calculate direction to move to
	const glm::vec3 v3DirectionToTarget = GetPointDirection(m_v3WanderPoint, v3SphereOrigin) * a_pUIValues->fInputWanderRadius.value;
	//Find out final target point
	m_v3WanderPoint = v3SphereOrigin + v3DirectionToTarget;
	//Add Jitter
	m_v3WanderPoint += glm::sphericalRand(a_pUIValues->fInputWanderJitter.value);

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
	const glm::vec3 v3OwnerPos = pOwnerTransform->GetCurrentPosition();

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
void BrainComponent::ApplyFlockingWeights(glm::vec3& a_v3SeparationForce, glm::vec3& a_v3AlignmentForce, glm::vec3& a_v3CohesionForce) const
{
	if(m_pDebugUI == nullptr)
	{
		return;
	}

	//Get the UI values
	UIInputValues* pForceValues = m_pDebugUI->GetUIInputValues();
		
	//Apply the UI weights to the forces
	a_v3SeparationForce *= pForceValues->fInputSeparationForce.value;
	a_v3AlignmentForce *= pForceValues->fInputAlignmentForce.value;
	a_v3CohesionForce *= pForceValues->fInputCohesionForce.value;
}

/// <summary>
/// Calculates both the containment and collision avoidance forces using a shared
/// raycast between both
/// </summary>
/// <returns>(Unweighted) The sum of the containent and collision avoidance forces</returns>
glm::vec3 BrainComponent::CalculateCollisionForces(glm::vec3& a_v3ContainmentForce, glm::vec3& a_v3CollisionAvoidForce) const
{

	//Check we have a collider component
	RaycastComponent* pRayCaster = m_pOwnerEntity->GetComponent<RaycastComponent*>();
	if(pRayCaster == nullptr)
	{
		return glm::vec3(0.f);
	}

	/*
	 * Do all of the raycats out and store their results then
	 * process pass the reults to the contain/avoid function
	 * so that they can check for indiviual types of collision
	 * (i.e containters for containment and obstacles for avoidance)
	 */
	const std::vector<rp3d::Ray*> vV3CollisionRays = GetCollisionRays();
	std::vector<RayCastHitsInfo*> vRayResults = pRayCaster->MutiRayCast(vV3CollisionRays, true);

	//Create a list of all of the hits from our results, so we only need to do this once and not
	//twice (for each containment and collision)
	std::vector<RayCastHit*> vAllRayHits;
	for(int resultsIndex = 0; resultsIndex < vRayResults.size(); ++resultsIndex)
	{
		RayCastHitsInfo* currentResult = vRayResults[resultsIndex];
		for(int hitIndex = 0; hitIndex < currentResult->m_vRayCastHits.size(); ++hitIndex)
		{
			//Add the hit to the list
			vAllRayHits.push_back(*&currentResult->m_vRayCastHits[hitIndex]);
		}

	}

	//Get forces from functions
	a_v3ContainmentForce = CalculateContainmentForce(vAllRayHits);
	a_v3CollisionAvoidForce = CalculateAvoidanceForce(vAllRayHits);
	
	//Delete all of the ray results, prevent mem leak
	for (int resultsIndex = 0; resultsIndex < vRayResults.size(); ++resultsIndex)
	{
		delete vRayResults[resultsIndex];
	}
	
	return a_v3CollisionAvoidForce + a_v3ContainmentForce;
}

/// <summary>
/// Calulates the amount of force needed to keep the boid
/// within the containment volume
/// </summary>
/// <returns>(Unweighted) force to turn away from hitting a container</returns>
glm::vec3 BrainComponent::CalculateContainmentForce(std::vector<RayCastHit*>& a_vRayCastHits) const
{
	//Store our containment force - init to 0 so we can return this var if we don't hit
	glm::vec3 v3ContainmentForce(0.0f, 0.0f, 0.0f);

	//Infomation about what we hit
	RayCastHit* containerHit = nullptr;
	float closestHitDist = 0.f; //Store the closest collision distance with the wall
	bool bHeadingForCollision = false;
	
	//Check our raycast hits if they have hit a containter then check it is the closest collision
	for(int i = 0; i < a_vRayCastHits.size(); ++i)
	{
		RayCastHit* currentHit = a_vRayCastHits[i];
		if(currentHit->m_pHitEntity->GetEntityType() == ENTITY_TYPE::ENTITY_TYPE_CONTAINER)
		{
			//Get the closest collision so we don't end up getting
			//the normal of the otherside of the containing wall
			const float hitDist = currentHit->m_fHitFraction;
			if (hitDist > closestHitDist)
			{
				containerHit = currentHit;
				bHeadingForCollision = true;
				closestHitDist = hitDist;
			}
		}
	}

	//If we have hit a container then calculate our force otherwise we will just return 0
	if (bHeadingForCollision)
	{
		//Get the normal and return our force in that direction so we turn away from the object,
		//mutiply it by the distance to the wall, so our force gets more agressive the closer we get
		//Invert the m_fHitFraction because 1 means it is a the very end of the ray, we want the opposite multiplication
		v3ContainmentForce = containerHit->m_v3HitNormal;
		v3ContainmentForce = glm::length(v3ContainmentForce) != 0 ? glm::normalize(v3ContainmentForce) : v3ContainmentForce;
		//v3ContainmentForce -= m_v3CurrentVelocity;
	}

	return v3ContainmentForce;
}

/// <summary>
/// Calculate the force needed to avoid any collision with obstacles
/// </summary>
/// <returns>(Unweighted) Force to avoid any collision</returns>
glm::vec3 BrainComponent::CalculateAvoidanceForce(std::vector<RayCastHit*>& a_vRayCastHits) const
{
	glm::vec3 v3AvoidForce(0.f);

	//Infomation about what we hit
	RayCastHit* containerHit = nullptr;
	float closestHitDist = 0.f; //Store the closest collision distance with the wall
	
	//Check our raycast hits if they have hit a containter then check it is the closest collision
	for (int i = 0; i < a_vRayCastHits.size(); ++i)
	{
		RayCastHit* currentHit = a_vRayCastHits[i];
		const ENTITY_TYPE hitType = currentHit->m_pHitEntity->GetEntityType();
		if (hitType == ENTITY_TYPE::ENTITY_TYPE_OBSTACLE || hitType == ENTITY_TYPE::ENTITY_TYPE_BOID)
		{
			//Get the closest collision so we don't end up getting
			//the normal of the otherside of the obstacle
			const float hitDist = currentHit->m_fHitFraction;
			if (hitDist > closestHitDist)
			{
				containerHit = currentHit;
				closestHitDist = hitDist;
			}
		}
	}
	
	//If we have hit a container then calculate our force otherwise we will just return 0
	if (containerHit != nullptr)
	{
		//Get the normal and return our force in that direction so we turn away from the object,
		//mutiply it by the distance to the wall, so our force gets more agressive the closer we get
		//Invert the m_fHitFraction because 1 means it is a the very end of the ray, we want the opposite multiplication
		v3AvoidForce = containerHit->m_v3HitNormal * (1 - containerHit->m_fHitFraction);
		v3AvoidForce = glm::length(v3AvoidForce) != 0 ? glm::normalize(v3AvoidForce) : v3AvoidForce;
	}
	

	return v3AvoidForce;
}

/// <summary>
/// Generates a list of rays to use when checking for collisions
/// </summary>
/// <returns>List of a rays the use for collision</returns>
std::vector<rp3d::Ray*> BrainComponent::GetCollisionRays() const
{
	//Create Vector
	std::vector<rp3d::Ray*> vRays;
	
	//Null Check and get transform
	if(!m_pOwnerEntity)
	{
		return vRays;
	}
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if(!pTransform)
	{
		return vRays;
	}
	
	//Get our current position, so we can make our rays relative
	const glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();

	//Number of directions we have (6 all directions around the boid)
	constexpr int iDirectionCount = 6;
	
	//Get all of the directions that we want to cast in
	const glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	const glm::vec3 v3Right = pTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	const glm::vec3 v3Up = pTransform->GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);
	glm::vec3 vV3PositiveDirections[3] = { v3Forward,v3Right,v3Up };

	//Loop through the directions and mutiply half of them by -1 so we have the inverse's
	for(int i = 0; i < iDirectionCount; ++i)
	{
		//Divide the current direction by 2 so that we grab
		//the direction twice and on 1 of those times * it by -1 so
		//we get the inverse
		const int iCurrentDirectionIndex = floor(i / 2);
		
		glm::vec3 v3CurrentRayDir = vV3PositiveDirections[iCurrentDirectionIndex];
		
		//If the index is odd then divide by 2 so we get the inerse
		if(i % 2 != 0)
		{
			v3CurrentRayDir *= -1;
		}

		//Create a ray from the direction and our current position
		const glm::vec3 v3EndPos = v3CurrentPos + (v3CurrentRayDir * m_fNeighbourRadius); //End pos is direction * distance, in this case our neighbourbood radius plus our velcityy
		rp3d::Ray* pCreatedRay = new rp3d::Ray(v3CurrentPos, v3EndPos);
		vRays.push_back(pCreatedRay);
	}

	return vRays;
}

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns>Name of the component</returns>
const char* BrainComponent::GetComponentName() const
{
	return m_szName;
}