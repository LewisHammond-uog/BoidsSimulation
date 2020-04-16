#include "BrainComponent.h"

//Project Incldues
#include "ColliderComponent.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "DebugUI.h"

//TODO Clean UP/Remove
//Constants
const float fMAX_SPEED = 5.0f;
const float fMAX_FORCE = 1.0f;
const float fNEIGHBOURHOOD_RADIUS = 5.0f;

const float fCIRCLE_FORWARD_MUTIPLIER = 1.f; //How far forward to draw the sphere
const float fJITTER = 0.5f; //How much to move from the point on the spehre
const float fWANDER_RADIUS = 4.0f; //How large the sphere is

BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner),
	m_v3CurrentVelocity(0.0f),
	m_v3WanderPoint(0.0f)
{
	m_eComponentType = BRAIN;
}

void BrainComponent::Update(float a_fDeltaTime)
{	
	//Get the Debug UI Instance so that we can get the user input values from it
	DebugUI* pDebugUI = DebugUI::GetInstance();

	//Get transform component
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Get vectors for calculation
	glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();;

	//Create an array of force values in the order that we want to apply them
	glm::vec3 afForceValues[] = {	CalculateSeperationForce() * pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_SEPERATION),
									CalculateAlignmentForce()* pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_ALIGNMENT),
									CalculateCohensionForce()* pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_COHESION),
									CalculateWanderForce(v3Forward, v3CurrentPos)* pDebugUI->GetUIFlockingWeight(FlockingBehaviourType::BEHAVIOUR_WANDER) };


	//Loop through all of the forces, only adding them if the do not excede the maximum force
	glm::vec3 v3NewForce(0.0f);
	for (glm::vec3 currentForceValue : afForceValues)
	{
		if(glm::length(v3NewForce + currentForceValue) < fMAX_FORCE)
		{
			v3NewForce += currentForceValue;
		}
	}

	//Apply Force
	m_v3CurrentVelocity += v3NewForce;
	m_v3CurrentVelocity = glm::clamp(m_v3CurrentVelocity, glm::vec3(-fMAX_FORCE, -fMAX_FORCE, -fMAX_FORCE), glm::vec3(fMAX_FORCE, fMAX_FORCE, fMAX_FORCE));
	//Apply Velocity to Position
	v3CurrentPos += m_v3CurrentVelocity * a_fDeltaTime;
	v3Forward = glm::length(m_v3CurrentVelocity) > 0.f ? glm::normalize(m_v3CurrentVelocity) : glm::vec3(0.f, 0.f, 1.f);
	 
	//Update our matrix
	pTransform->SetEntityMatrixRow(FORWARD_VECTOR, v3Forward);
	pTransform->SetEntityMatrixRow(POSTION_VECTOR, v3CurrentPos);
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

glm::vec3 BrainComponent::CalculateWanderForce(const glm::vec3& v3Forward, const glm::vec3& v3CurrentPos)
{
	//Project a point in front of us for the center of our sphere
	const glm::vec3 v3SphereOrigin = v3CurrentPos + (v3Forward * fCIRCLE_FORWARD_MUTIPLIER);

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

//TODO - Make nicer (i.e more generic code) - All in 1 funct?
glm::vec3 BrainComponent::CalculateSeperationForce()
{
	//Init Values
	glm::vec3 v3SeperationVelocity(0.0f);
	unsigned int uNeightbourCount = 0;

	//Check that we have an owner entity
	if (!GetOwnerEntity()) {
		return glm::vec3();
	}

	//Get the transform
	TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pLocalTransform) {
		return glm::vec3();
	}

	const glm::vec3 v3LocalPos = pLocalTransform->GetCurrentPosition();

	//Itterate over every entity in the scene
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter) 
	{
		//Current Entity
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Check that the entity we have is not the owner of this brain component
		if (pTarget->GetEntityID() == GetOwnerEntity()->GetEntityID()) {
			continue;
		}

		
		//Find the distance between this entity and the target entity
		TransformComponent* pTargetTransform = pTarget->GetComponent<TransformComponent*>();
		if (!pTargetTransform) {
			continue;
		}
		
		glm::vec3 v3TargetPos = pTargetTransform->GetCurrentPosition();
		const float fDistance = glm::length(v3TargetPos - v3LocalPos);

		//Check that object is within checking range and add velocity
		if (fDistance < fNEIGHBOURHOOD_RADIUS) {
			v3SeperationVelocity += v3LocalPos - v3TargetPos;
			uNeightbourCount++;
		}
	}

	if (glm::length(v3SeperationVelocity) > 0) {
		v3SeperationVelocity /= uNeightbourCount;
		v3SeperationVelocity = glm::normalize(v3SeperationVelocity);
	}

	return v3SeperationVelocity;
}

//TODO - Make nicer (i.e more generic code) - All in 1 funct?
glm::vec3 BrainComponent::CalculateAlignmentForce()
{
	//Init Values
	glm::vec3 v3AlignmentVelocity(0.0f);//TODO Vec3 for average position? seperate from this?
	unsigned int uNeightbourCount = 0;

	//Check that we have an owner entity
	if (!GetOwnerEntity()) {
		return glm::vec3();
	}

	//Get the transform
	TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pLocalTransform) {
		return glm::vec3();
	}

	const glm::vec3 v3LocalPos = pLocalTransform->GetCurrentPosition();

	//TODO FUNCTION FOR LIST OF ALL ENTITYS/NEIGHBOURS
	//Itterate over every entity in the scene
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Current Entity
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Check that the entity we have is not the owner of this brain component
		if (pTarget->GetEntityID() == GetOwnerEntity()->GetEntityID()) {
			continue;
		}


		//Find the distance between this entity and the target entity
		TransformComponent* pTargetTransform = pTarget->GetComponent<TransformComponent*>();
		BrainComponent* pTargetBrain = pTarget->GetComponent<BrainComponent*>();
		if (!pTargetTransform || !pTargetBrain) {
			continue;
		}

		glm::vec3 v3TargetPos = pTargetTransform->GetCurrentPosition();
		const float fDistance = glm::length(v3TargetPos - v3LocalPos);

		//Check that object is within checking range and add velocity
		if (fDistance < fNEIGHBOURHOOD_RADIUS) {
			v3AlignmentVelocity += pTargetBrain->GetCurrentVelocity();
			uNeightbourCount++;
		}
	}

	if (glm::length(v3AlignmentVelocity) > 0) {
		v3AlignmentVelocity /= uNeightbourCount;
		v3AlignmentVelocity = glm::normalize(v3AlignmentVelocity);
	}

	return v3AlignmentVelocity;
}

glm::vec3 BrainComponent::CalculateCohensionForce()
{
	//Init Values
	glm::vec3 v3CohesionVelocity(0.0f);
	unsigned int uNeightbourCount = 0;

	//Check that we have an owner entity
	if (!GetOwnerEntity()) {
		return glm::vec3();
	}

	//Get the transform
	TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pLocalTransform) {
		return glm::vec3();
	}

	const glm::vec3 v3LocalPos = pLocalTransform->GetCurrentPosition();

	//Itterate over every entity in the scene
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Current Entity
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Check that the entity we have is not the owner of this brain component
		if (pTarget->GetEntityID() == GetOwnerEntity()->GetEntityID()) {
			continue;
		}


		//Find the distance between this entity and the target entity
		TransformComponent* pTargetTransform = pTarget->GetComponent<TransformComponent*>();
		if (!pTargetTransform) {
			continue;
		}

		glm::vec3 v3TargetPos = pTargetTransform->GetCurrentPosition();
		const float fDistance = glm::length(v3TargetPos - v3LocalPos);

		//Check that object is within checking range and add velocity
		if (fDistance < fNEIGHBOURHOOD_RADIUS) {
			v3CohesionVelocity += v3TargetPos;
			uNeightbourCount++;
		}
	}

	if (glm::length(v3CohesionVelocity) > 0) {
		v3CohesionVelocity /= uNeightbourCount;
		v3CohesionVelocity = glm::normalize(v3CohesionVelocity - v3LocalPos);
	}

	return v3CohesionVelocity;
}
