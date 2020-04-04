#include "ColliderComponent.h"

//Gizmos Includes
#include <Gizmos.h>

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"
#include "DebugUI.h"

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
}

ColliderComponent::~ColliderComponent()
{
	//Destroy the collision body
	delete m_pCollisionBody;

	//Delete all of the collision and proxy shapes
	for (unsigned int i =0; i < m_apCollisionShapes.size(); ++i)
	{
		delete m_apCollisionShapes[i];
	}
	for (unsigned int i = 0; i < m_apCollisionShapes.size(); ++i)
	{
		delete m_apProxyShapes[i];
	}
}

/// <summary>
/// Update the transform of the collider
/// </summary>
/// <param name="a_fDeltaTime"></param>
void ColliderComponent::Update(float a_fDeltaTime)
{
	//Get new position and transform of the physics body
	TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->GetComponent(COMPONENT_TYPE::TRANSFORM));//TODO Make this Nicer
	if (pLocalTransform == nullptr)
	{
		return;
	}
	m_pCollisionBody->setTransform(GetPhysicsTransform(pLocalTransform));
}

void ColliderComponent::Draw(Shader* a_pShader)
{
	if (DebugUI::GetInstance()->GetShowColliders()) {
		//Get the current position of the object - all colliders positions are relative
		//to this
		TransformComponent* pLocalTransform = static_cast<TransformComponent*>(GetOwnerEntity()->GetComponent(COMPONENT_TYPE::TRANSFORM));
		if (pLocalTransform == nullptr)
		{
			return;
		}
		glm::vec3 v3CurrentPosition = pLocalTransform->GetCurrentPosition();

		//Draw all of the sub-colliders that are part of this collider
		for (unsigned int i = 0; i < m_apCollisionShapes.size(); ++i)
		{
			switch (m_apCollisionShapes[i]->getName())
			{
			case reactphysics3d::CollisionShapeName::BOX: {
				//TODO Cleanup
				rp3d::BoxShape* pBox = dynamic_cast<reactphysics3d::BoxShape*>(m_apCollisionShapes[i]);
				glm::vec3 v3boxCenter = glm::vec3(pBox->getCentroid().x, pBox->getCentroid().y, pBox->getCentroid().z);
				glm::vec3 v3boxDimentions = glm::vec3(pBox->getExtent().x, pBox->getExtent().y, pBox->getExtent().z);
				Gizmos::addBox(v3CurrentPosition + v3boxCenter, v3boxDimentions, true);
				break;
			}

			case reactphysics3d::CollisionShapeName::SPHERE: {
				//TODO Cleanup
				rp3d::SphereShape* pSphere = dynamic_cast<reactphysics3d::SphereShape*>(m_apCollisionShapes[i]);
				const float fSphereRadius = pSphere->getRadius();
				Gizmos::addSphere(v3CurrentPosition, 10, 10, fSphereRadius, glm::vec4(1.f));
				break;
			}
			default:
				//Default do nothing as we don't have a shape to draw
				break;
			}
		}
	}
	
}


/// <summary>
/// Add a box collider to the collision component
/// </summary>
/// <param name="a_v3BoxSize">Size of the box to add</param>
/// <param name="a_v3Offset">Local Offset from the center of the object to put the collider</param>
void ColliderComponent::AddBoxCollider(const glm::vec3 a_v3BoxSize, const glm::vec3 a_v3Offset)
{
	//Create a rp3d vector to store the extends (which are half the box size)
	//then create the box shape itself
	const rp3d::Vector3 v3BoxExtends(a_v3BoxSize);
	rp3d::BoxShape* pBoxShape = new rp3d::BoxShape(v3BoxExtends);

	//Create a proxy shape to link the transform with the box
	const rp3d::Transform boxTransform(a_v3Offset, rp3d::Quaternion::identity());
	rp3d::ProxyShape* pBoxProxyState = m_pCollisionBody->addCollisionShape(pBoxShape, boxTransform);
	
	//Add the box and proxy shape to our list
	m_apCollisionShapes.push_back(pBoxShape);
	m_apProxyShapes.push_back(pBoxProxyState);
}

/// <summary>
/// Add a sphere collider to the collision component
/// </summary>
/// <param name="a_fSphereSize">Size of the sphere to add</param>
/// <param name="a_v3Offset">Local Offset from the center of the object to put the collider</param>
void ColliderComponent::AddSphereCollider(const float a_fSphereSize, const glm::vec3 a_v3Offset)
{
	//Create the sphere shape
	rp3d::SphereShape* pSphereShape = new rp3d::SphereShape(a_fSphereSize);

	//Create a proxy shape to link the transform with the box
	const rp3d::Transform sphereTransform(a_v3Offset, rp3d::Quaternion::identity());
	rp3d::ProxyShape* pSphereProxyShape = m_pCollisionBody->addCollisionShape(pSphereShape, sphereTransform);

	//Add the sphere and proxy shape to our list
	m_apCollisionShapes.push_back(pSphereShape);
	m_apProxyShapes.push_back(pSphereProxyShape);
}

/// <summary>
/// Gets if we are colliding with any object in the world
/// </summary>
/// <param name="a_bUseAABB">Whether we should use AABB testing for collisions, rather than more
/// precise methods (i.e mesh)</param>
/// <returns></returns>
bool ColliderComponent::IsColliding(const bool a_bUseAABB)
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

/// <summary>
/// Gets all of the Collision Info about all of the collisions that occour with this
/// object this frame
/// </summary>
/// <returns>A list of collision info's with every object that collided with this one
/// this frame</returns>
std::vector<CollisionInfo> ColliderComponent::GetCollisionInfo()
{
	//Store the current collision info in a vector
	std::vector<CollisionInfo> vObjectCollisions;
	
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
		CollisionInfo currentCollision = GetCollisionInfo(pTargetCollider);
		//TODO Make this check that collision is valid
		vObjectCollisions.push_back(currentCollision);
	}

	//Return the list of collisions, this may be empty if no
	//collisions have occoured
	return vObjectCollisions;
}

/// <summary>
/// Get's the collision info between this and another specified collider 
/// </summary>
/// <param name="a_pOtherCollider">Other Collider to test</param>
/// <returns>Collision Info about collision, which may be null if no collision occoured or
/// nullptr if the collision parameters are invalid</returns>
CollisionInfo ColliderComponent::GetCollisionInfo(ColliderComponent* a_pOtherCollider) const
{

	//Check if the collision check is valid - all pointers are not null
	if (IsCollisionCheckValid(a_pOtherCollider)) {

		//Return the collision info from our collision test - this will
		//be nullptr if no collision occours
		CollisionInfo collisionInfo = CollisionInfo();
		m_pCollisionWorld->testCollision(m_pCollisionBody, a_pOtherCollider->m_pCollisionBody, &collisionInfo);
		return collisionInfo;
	}

	//Collision check is invalid - return blank collision info
	return CollisionInfo();
}

/// <summary>
/// Coverts a TransformComponent to a rp3d Transform
/// </summary>
/// <param name="a_pTransform">Transform Component to convert</param>
/// <returns>rp3d Transform</returns>
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

/// <summary>
/// Checks if a collision check will be valid by checking that
/// both the collision world an other collider exists
/// </summary>
/// <param name="a_pOtherCollider">Other to collider in expected collision</param>
/// <returns>If the collision check will be valid</returns>
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

/// <summary>
/// Function to implement required virtual function in CollisionCallbackInfo class
/// Triggered whenever we detect a collision between 2 collision bodies.
/// </summary>
/// <param name="a_pCollisionCallbackInfo"></param>
void CollisionInfo::notifyContact(const CollisionCallbackInfo& a_pCollisionCallbackInfo)
{
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

	//Loop through all of the entites that we have
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Get the current entity that are on and check that it is not nullptr
		Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Get the collider component
		ColliderComponent* pTargetCollider = static_cast<ColliderComponent*>(pTarget->GetComponent(COMPONENT_TYPE::COLLIDER));
		if (pTargetCollider == nullptr) {
			continue;
		}

		//If this object's collider component's physics body matches any of the bodies that we have collided
		//with then add it as a entity involved in the collision
		if(a_pCollisionCallbackInfo.body1 == pTargetCollider->m_pCollisionBody ||
			a_pCollisionCallbackInfo.body2 == pTargetCollider->m_pCollisionBody)
		{
			m_aCollisionEntities.push_back(pTarget);
		}
		
	}
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