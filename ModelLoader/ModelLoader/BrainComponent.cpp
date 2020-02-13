#include "BrainComponent.h"

//Project Incldues
#include "Entity.h"
#include "TransformComponent.h"

//Constants
static const float cfSPEED = 2.0f;

static const float fCIRCLE_FORWARD_MUTIPLIER = 1.f; //How far forward to draw the sphere
static const float fJITTER = 0.5f; //How much to move from the point on the spehre
static const float fWANDER_RADIUS = 4.0f; //How large the sphere is

BrainComponent::BrainComponent(Entity* a_pOwner)
	: Component(a_pOwner),
	m_v3CurrentVelocity(0.0f),
	m_v3WanderPoint(0.0f)
{
	m_eComponentType = BRAIN;
}

void BrainComponent::Update(float a_fDeltaTime)
{
	//Get our entity owner
	Entity* pEntity = GetOwnerEntity();
	if (!pEntity) {
		return;
	}

	//Get transform component
	TransformComponent* pTransform = static_cast<TransformComponent*>(pEntity->GetComponent(TRANSFORM));
	if (!pTransform) {
		return;
	}

	//Get vectors for calculation
	glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	glm::vec3 v3CurrentPos = pTransform->GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);

	//Calcualte forces
	glm::vec3 v3NewForce(0.0f);
	//Seek
	//v3NewForce = CalculateSeekForce(glm::vec3(4.0f, 0.0f, 4.0f), v3CurrentPos);
	//Flee
	//v3NewForce = CalculateFleeForce(glm::vec3(4.0f, 0.0f, 4.0f), v3CurrentPos);
	//Wander
	v3NewForce = CalculateWanderForce(v3Forward, v3CurrentPos);

	//Apply Force
	m_v3CurrentVelocity += v3NewForce;

	//Clamp Velocity
	//TODO Make this not use magic numbers
	m_v3CurrentVelocity = glm::clamp(m_v3CurrentVelocity, glm::vec3(-10.0f, 0.0f, -10.0f), glm::vec3(10.0f, 0.0f, 10.0f));

	//Apply Velocity to Position
	v3CurrentPos += m_v3CurrentVelocity * a_fDeltaTime;
	
	//Recalculate matrix rows - only normalise if not a zero vector
	v3Forward = m_v3CurrentVelocity;
	if (glm::length(v3Forward) > 0.0f) {
		v3Forward = glm::normalize(v3Forward);
	}

	//TODO make this calculate rather than what it currently is
	glm::vec3 v3Up = pTransform->GetEntityMatrixRow(UP_VECTOR);
	glm::vec3 v3Right = glm::cross(v3Up, v3Forward);

	//Update our matrix
	pTransform->SetEntityMatrixRow(RIGHT_VECTOR, v3Right);
	pTransform->SetEntityMatrixRow(FORWARD_VECTOR, v3Forward);
	pTransform->SetEntityMatrixRow(POSTION_VECTOR, v3CurrentPos);

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
	glm::vec3 v3NewVelocity = v3TargetDir * cfSPEED;

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
	glm::vec3 v3NewVelocity = v3TargetDir * cfSPEED;

	//Force is target velocity - current velocity
	return (v3NewVelocity - m_v3CurrentVelocity);
}

glm::vec3 BrainComponent::CalculateWanderForce(const glm::vec3& v3Forward, const glm::vec3& v3CurrentPos)
{
	//Project a point in front of us for the center of our sphere
	glm::vec3 v3SphereOrigin = v3CurrentPos + (v3Forward * fCIRCLE_FORWARD_MUTIPLIER);

	//If the magnitude of the vector is 0 then initalize our
	//first wander point
	if (glm::length(m_v3WanderPoint) == 0.0f) 
	{
		//Find a random point omn a sphere
		glm::vec3 v3RandomPointOnSphere = glm::sphericalRand(fWANDER_RADIUS);

		//Add this point on a sphere to the sphere we are casting out infront of us
		m_v3WanderPoint = v3SphereOrigin + v3RandomPointOnSphere;
	}

	//Calculate direction to move to
	glm::vec3 v3DirectionToTarget = glm::normalize(m_v3WanderPoint - v3SphereOrigin) * fWANDER_RADIUS;

	//Find out final target point
	m_v3WanderPoint = v3SphereOrigin + v3DirectionToTarget;

	//Add Jitter
	m_v3WanderPoint += glm::sphericalRand(fJITTER);

	return CalculateSeekForce(m_v3WanderPoint, v3CurrentPos);
}
