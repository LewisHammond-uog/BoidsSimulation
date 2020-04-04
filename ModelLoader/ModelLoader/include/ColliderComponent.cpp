#include "ColliderComponent.h"

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"

//Typedefs
typedef Component PARENT;


ColliderComponent::ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld) :
	PARENT(a_pOwner),
	m_pCollisionWorld(a_pCollisionWorld)
{
	m_eComponentType = COLLIDER;

	/*
	* Create the collision body, used by rp3d at the transform of
	* the entity, then attach a collision shape so that we can
	* collide with other objects
	*/

	//Create collision body at object transform
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->GetComponent(COMPONENT_TYPE::TRANSFORM));
	m_pCollisionBody = m_pCollisionWorld->createCollisionBody(GetPhysicsTransform(pLocalTransform));

	//TO DO Cleanup
	//Create a collision shape
	rp3d::decimal fShapeRadius = rp3d::decimal(m_fColliderRadius);
	m_pCollisionShape = new rp3d::SphereShape(fShapeRadius);
	//Transform the collision the shape at the origin of the body local-space 
	rp3d::Transform transform = rp3d::Transform::identity();
	//0 Mass Collision Shape
	rp3d::decimal mass = rp3d::decimal(0.f);
	//Add the shape to the collision body - proxy shape is not copied to needs to be maintained properly
	m_pProxyShape = m_pCollisionBody->addCollisionShape(m_pCollisionShape, transform);
}

ColliderComponent::~ColliderComponent()
{
	//Destory the collision body
	if (m_pCollisionBody != nullptr) {
		delete m_pCollisionBody;
	}

	//We don't need to delete the pointer to the collision shape as 
	//"It is not necessary to manually remove all the collision shapes 
	//from a body at the end of your application. They will automatically 
	//be removed when you destroy the body." - Section 10.7 (https://www.reactphysics3d.com/usermanual.html#x1-4500010.7)
}

/// <summary>
/// Update the transform of the collider
/// </summary>
/// <param name="a_fDeltaTime"></param>
void ColliderComponent::Update(float a_fDeltaTime)
{
	//Get new position and transform of the physics body
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->GetComponent(COMPONENT_TYPE::TRANSFORM));//TODO Make this Nicer
	m_pCollisionBody->setTransform(GetPhysicsTransform(pLocalTransform));

	GetCollisionInfo();
}

void ColliderComponent::Draw(Shader* a_pShader)
{
	//Blank
}

/// <summary>
/// Gets if we are colliding with any object in the world
/// </summary>
/// <param name="a_bUseAABB">Whether we should use AABB testing for collisions, rather than more
/// precise methods (i.e mesh)</param>
/// <returns></returns>
bool ColliderComponent::IsColliding(bool a_bUseAABB)
{
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

	//TODO - Make Nicer
	//Loop through all of the entites that we have
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Get the current entity that are on and check that it is not nullptr
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Check that our target entity is not ourself, as we should not check
		//if we are collding with ourselves
		if (pTarget->GetEntityID() == GetOwnerEntity()->GetEntityID()) {
			continue;
		}

		//Get the collider component
		ColliderComponent* pTargetCollider = static_cast<ColliderComponent*>(pTarget->GetComponent(COMPONENT_TYPE::COLLIDER));
		if (pTargetCollider == nullptr) {
			return false;
		}

		//If we find a collision return true
		if (IsColliding(pTargetCollider, a_bUseAABB)) {
			return true;
		}

	}

	//We have reached the end of the loop and not found a collision,
	//thus return false
	return false;
}


/// <summary>
/// Checks if this object is colliding with another collision body
/// </summary>
/// <param name="a_pOtherCollider">The other collider to check</param>
/// <param name="a_bUseAABB">Whether we should use AABB testing for collisions, rather than more
/// precise methods (i.e mesh)</param>
/// <returns>If a collision is occouring</returns>
bool ColliderComponent::IsColliding(ColliderComponent* a_pOtherCollider, bool a_bUseAABB) const
{
	//Check if the collision check is valid - all pointers are not null
	if (IsCollisionCheckValid(a_pOtherCollider)) {

		//Choose which methold to use by the users selection (to use AABB or not)
		//then perform the collision check
		if (a_bUseAABB) {
			return m_pCollisionBody->testAABBOverlap(a_pOtherCollider->m_pCollisionBody->getAABB());
		}
		else {
			return m_pCollisionWorld->testOverlap(m_pCollisionBody, a_pOtherCollider->m_pCollisionBody);
		}
	}

	//Collision check is invalid - return false
	return false;
}

std::vector<CollisionInfo*> ColliderComponent::GetCollisionInfo()
{
	//Store the current collision info
	std::vector<CollisionInfo*> objectCollisions;
	
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

	//TODO - Make Nicer
	//Loop through all of the entites that we have
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Get the current entity that are on and check that it is not nullptr
		const Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Check that our target entity is not ourself, as we should not check
		//if we are collding with ourselves
		if (pTarget->GetEntityID() == GetOwnerEntity()->GetEntityID()) {
			continue;
		}

		//Get the collider component
		ColliderComponent* pTargetCollider = static_cast<ColliderComponent*>(pTarget->GetComponent(COMPONENT_TYPE::COLLIDER));
		if (pTargetCollider == nullptr) {
			continue;
		}

		//If we find a collision then add it to our list
		CollisionInfo* currentCollision = GetCollisionInfo(pTargetCollider);
		if(!currentCollision->m_vContactList.empty())
		{
			objectCollisions.push_back(currentCollision);
		}

	}

	//Return the list of collisions, this may be empty if no
	//collisions have occoured
	return objectCollisions;
}

CollisionInfo* ColliderComponent::GetCollisionInfo(ColliderComponent* a_pOtherCollider) const
{

	//Check if the collision check is valid - all pointers are not null
	if (IsCollisionCheckValid(a_pOtherCollider)) {

		//Return the collision info from our collision test - this will
		//be nullptr if no collision occours
		CollisionInfo* collisionInfo = new CollisionInfo();
		m_pCollisionWorld->testCollision(m_pCollisionBody, a_pOtherCollider->m_pCollisionBody, collisionInfo);
		return collisionInfo;
	}

	//Collision check is invalid - return nullptr
	return nullptr;
}

rp3d::Transform ColliderComponent::GetPhysicsTransform(TransformComponent* a_pTransform)
{
	//Early out, returning a default 
	//transform if we were not given a valid transform
	if (a_pTransform == nullptr) {
		return rp3d::Transform::identity();
	}

	//Convert Position Component
	const glm::vec3 v3TransformPos = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);
	const rp3d::Vector3 v3PosVector = rp3d::Vector3(v3TransformPos.x, v3TransformPos.y, v3TransformPos.z);

	//Convert Rotation Component
	const glm::vec3 v3Right = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	const glm::vec3 v3Up = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);
	const glm::vec3 v3Forward = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	const rp3d::Matrix3x3 m3RotVector = rp3d::Matrix3x3(v3Right.x, v3Right.y, v3Right.z,
		v3Up.x, v3Up.y, v3Up.z,
		v3Forward.x, v3Forward.y, v3Up.z);


	return rp3d::Transform(v3PosVector, m3RotVector);
}

bool ColliderComponent::IsCollisionCheckValid(ColliderComponent* a_pOtherCollider) const
{
	//Check that the collision world exists
	if (m_pCollisionWorld == nullptr) {
		return false;
	}

	//Null check the collider we have been passed - return
	//false aas we cannot have a collision with a collider that 
	//does not exist
	if (a_pOtherCollider == nullptr) {
		return false;
	}

	//Check that the collider component has a collision body - no
	//collision without a colliision body
	if (a_pOtherCollider->m_pCollisionBody == nullptr) {
		return false;
	}

	//All checks passed
	return true;
}

void CollisionInfo::notifyContact(const CollisionCallbackInfo& collisionCallbackInfo)
{
	m_vContactList.push_back(collisionCallbackInfo);
}


/*
rp3d::RaycastInfo* ColliderComponent::RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint)
{
	//Create a ray from the given start and end point
	const rp3d::Vector3 v3StartPoint(a_v3StartPoint.x, a_v3StartPoint.y, a_v3StartPoint.z);
	const rp3d::Vector3 v3EndPoint(a_v3EndPoint.x, a_v3EndPoint.y, a_v3EndPoint.z);
	rp3d::Ray raycastRay(v3StartPoint, v3EndPoint);

	//Call function that takes ray as parameter and return the result of that
	return RayCast(&raycastRay);
}

rp3d::RaycastInfo* ColliderComponent::RayCast(rp3d::Ray* a_Ray)
{
	//Create raycast info for the result
	rp3d::RaycastInfo* raycastInfo = new rp3d::RaycastInfo();

	//Perform Raycast and return the resulting info
	m_pCollisionBody->raycast(*a_Ray, *raycastInfo);
	return raycastInfo;
}
*/