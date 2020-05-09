#include "ColliderComponent.h"

//Gizmos Includes
#include <Gizmos.h>

//Project Includes
#include "TransformComponent.h"
#include "Entity.h"
#include "DebugUI.h"

//Typedefs
typedef Component PARENT;

/// <summary>
/// Create a collider component within a given collision world
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
/// <param name="a_pCollisionWorld">Collision World this collider exists in</param>
ColliderComponent::ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld) :
	PARENT(a_pOwner),
	m_pCollisionWorld(a_pCollisionWorld)
{
	/*
	* Create the collision body, used by rp3d at the transform of
	* the entity, then attach a collision shape so that we can
	* collide with other objects
	*/

	//Create collision body at object transform, or default transform if we don't have one
	if (m_pCollisionWorld != nullptr) {
		if (m_pOwnerEntity != nullptr) {
			TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
			if (pLocalTransform) {
				m_pCollisionBody = m_pCollisionWorld->createCollisionBody(pLocalTransform);
			}else
			{
				m_pCollisionBody = m_pCollisionWorld->createCollisionBody(rp3d::Transform::identity());
			}
		}
	}
}

/// <summary>
/// Destroy the collider component
/// </summary>
ColliderComponent::~ColliderComponent()
{

	//Delete all shapes
	for(int i = 0; i < m_apCollisionShapes.size(); ++i)
	{
		delete m_apCollisionShapes[i];
	}
	m_apCollisionShapes.clear();
	
	m_pCollisionWorld->destroyCollisionBody(m_pCollisionBody);

	//Proxy and Collision Shapes are removed by React Physics, we do not need to delete them here
	/* It is not necessary to manually remove all the collision shapes from a body at the end of your application.
	 * They will automatically be removed when you destroy the body.
	 * Note that after removing a collision shape, the corresponding proxy shape pointer will not be valid anymore
	 * https://www.reactphysics3d.com/usermanual.html#x1-4500010.7
	 */
}

/// <summary>
/// Update the transform of the collider
/// </summary>
/// <param name="a_fDeltaTime"></param>
void ColliderComponent::Update(float a_fDeltaTime)
{
	//Get new position and transform of the physics body
	TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();	
	if (pLocalTransform == nullptr)
	{
		return;
	}
	if (m_pCollisionBody) {
		m_pCollisionBody->setTransform(pLocalTransform);
	}
}

/// <summary>
/// Draw the collider shapes of this component
/// Only drawn when the tickbox is selected in the debug UI
/// </summary>
/// <param name="a_pShader"></param>
void ColliderComponent::Draw(Shader* a_pShader)
{
	if (DebugUI::GetInstance()->GetUIInputValues()->bShowColliders) {
		
		//Get the current position of the object - all colliders positions are relative
		//to this
		TransformComponent* pLocalTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
		if (pLocalTransform == nullptr)
		{
			return;
		}
		const glm::vec3 v3CurrentPosition = pLocalTransform->GetCurrentPosition();

		//Draw all of the sub-colliders that are part of this collider
		for (unsigned int i = 0; i < m_apCollisionShapes.size(); ++i)
		{
			switch (m_apCollisionShapes[i]->getName())
			{
			case reactphysics3d::CollisionShapeName::BOX: {
				//Draw a box at the collider point
				rp3d::BoxShape* pBox = dynamic_cast<reactphysics3d::BoxShape*>(m_apCollisionShapes[i]);
				glm::vec3 v3BoxCenter = glm::vec3(pBox->getCentroid().x, pBox->getCentroid().y, pBox->getCentroid().z);
				glm::vec3 v3BoxDimensions = glm::vec3(pBox->getExtent().x, pBox->getExtent().y, pBox->getExtent().z);
				Gizmos::addBox(v3CurrentPosition + v3BoxCenter, v3BoxDimensions, true, mc_v4ColliderDrawCol);
				break;
			}

			case reactphysics3d::CollisionShapeName::SPHERE: {
				//Draw a sphere at the collider point
				rp3d::SphereShape* pSphere = dynamic_cast<reactphysics3d::SphereShape*>(m_apCollisionShapes[i]);
				const float fSphereRadius = pSphere->getRadius();
				Gizmos::addSphere(v3CurrentPosition, mc_iColliderDrawRes, mc_iColliderDrawRes, fSphereRadius, mc_v4ColliderDrawCol);
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
	if(!m_pCollisionBody || !m_pCollisionWorld )
	{
		return;
	}
	
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
	if (!m_pCollisionBody || !m_pCollisionWorld)
	{
		return;
	}
	
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
/// <returns>If we are colliding with any other object in the scene</returns>
bool ColliderComponent::IsColliding(const bool a_bUseAABB) const
{
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

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
		ColliderComponent* pTargetCollider = pTarget->GetComponent<ColliderComponent*>();
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
/// IMPORTANT: You must delete any collision info that is returned by this function
/// </summary>
/// <returns>A list of collision info's with every object that collided with this one
/// this frame</returns>
std::vector<CollisionInfo*> ColliderComponent::GetCollisionInfo() const
{
	//Store the current collision info in a vector
	std::vector<CollisionInfo*> vObjectCollisions;
	
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

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
		ColliderComponent* pTargetCollider = pTarget->GetComponent<ColliderComponent*>();
		if (pTargetCollider == nullptr) {
			continue;
		}

		//If we find a collision then add it to our list
		CollisionInfo* currentCollision = GetCollisionInfo(pTargetCollider);
		if (currentCollision != nullptr) {
			vObjectCollisions.push_back(currentCollision);
		}
	}

	//Return the list of collisions, this may be empty if no
	//collisions have occoured
	return vObjectCollisions;
}

/// <summary>
/// Get's the collision info between this and another specified collider
/// IMPORTANT: You must delete any collision info that is returned by this function
/// </summary>
/// <param name="a_pOtherCollider">Other Collider to test</param>
/// <returns>Collision Info about collision, which may be null if no collision occoured or
/// nullptr if the collision parameters are invalid</returns>
CollisionInfo* ColliderComponent::GetCollisionInfo(ColliderComponent* a_pOtherCollider) const
{

	//Check if the collision check is valid - all pointers are not null
	if (IsCollisionCheckValid(a_pOtherCollider)) {

		//Return the collision info from our collision test - this will
		//be nullptr if no collision occours
		CollisionInfo* pCollisionInfo =  new CollisionInfo();
		m_pCollisionWorld->testCollision(m_pCollisionBody, a_pOtherCollider->m_pCollisionBody, pCollisionInfo);
		return pCollisionInfo;
	}

	//Collision check is invalid - return blank collision info
	return nullptr;
}

/// <summary>
/// Checks if a collision check will be valid by checking that
/// both the collision world an other collider exists
/// </summary>
/// <param name="a_pOtherCollider">Other to collider in expected collision</param>
/// <returns>If the collision check will be valid</returns>
bool ColliderComponent::IsCollisionCheckValid(ColliderComponent* a_pOtherCollider) const
{
	//Check that the collision world and collision body exists
	if (m_pCollisionWorld == nullptr || m_pCollisionBody == nullptr) {
		return false;
	}

	//Null check the collider we have been passed - return
	//false aas we cannot have a collision with a collider that 
	//does not exist
	if (a_pOtherCollider == nullptr) {
		return false;
	}

	//Check that the collider component has a collision body - no
	//collision without a collision body
	if (a_pOtherCollider->m_pCollisionBody == nullptr) {
		return false;
	}

	//All checks passed
	return true;
}

/// <summary>
/// Gets the entity attached to a given collision body
/// </summary>
/// <param name="a_collisionBody">Collision Body to get entity from</param>
/// <returns>Entity attached to collision body</returns>
Entity* ColliderComponent::GetEntityFromCollisionBody(rp3d::CollisionBody* a_collisionBody)
{
	//Create a map to itterate through all of our entities
	const std::map<const unsigned int, Entity*>& xEntityMap = Entity::GetEntityMap();
	std::map<const unsigned int, Entity*>::const_iterator xIter;

	//Null check the collision body
	if(a_collisionBody == nullptr)
	{
		return nullptr;
	}
	
	//Loop through all of the entites that we have
	for (xIter = xEntityMap.begin(); xIter != xEntityMap.end(); ++xIter)
	{
		//Get the current entity that are on and check that it is not nullptr
		Entity* pTarget = xIter->second;
		if (!pTarget) {
			continue;
		}

		//Get the collider component
		ColliderComponent* pTargetCollider = pTarget->GetComponent<ColliderComponent*>();
		if (pTargetCollider == nullptr) {
			continue;
		}
		if(pTargetCollider->m_pCollisionBody == nullptr)
		{
			continue;
		}

		//If this object's collider component's physics body matches the one we
		//are looking for then return it
		if (a_collisionBody == pTargetCollider->m_pCollisionBody)
		{
			return pTarget;
		}

	}

	//There is no entity attached to this collision body
	return nullptr;
}

/// <summary>
/// Ray casts between 2 points in the world
/// </summary>
/// <param name="a_v3StartPoint">World Start Point</param>
/// <param name="a_v3EndPoint">World End Point</param>
/// <returns>Infomation about all of the objects that the raycast has hit</returns>
RayCastHitsInfo* ColliderComponent::RayCast(const glm::vec3 a_v3StartPoint, const glm::vec3 a_v3EndPoint) const
{
	//Create a ray from the given start and end point
	const rp3d::Vector3 v3StartPoint(a_v3StartPoint.x, a_v3StartPoint.y, a_v3StartPoint.z);
	const rp3d::Vector3 v3EndPoint(a_v3EndPoint.x, a_v3EndPoint.y, a_v3EndPoint.z);
	rp3d::Ray raycastRay(v3StartPoint, v3EndPoint);

	//Call function that takes ray as parameter and return the result of that
	return RayCast(&raycastRay);
}

RayCastHitsInfo* ColliderComponent::RayCast(rp3d::Ray* a_ray) const
{
	RayCastHitsInfo* callback = new RayCastHitsInfo();

	//Perform Raycast and return the resulting info
	m_pCollisionWorld->raycast(*a_ray, callback);
	return callback;
}

/// <summary>
/// Performs Mutiple Raycasts and returns all of the hits from those casts
/// </summary>
/// <param name="a_vRays">Rays to process</param>
/// <returns>All of the Hits from the raycasts</returns>
std::vector<RayCastHitsInfo*> ColliderComponent::MutiRayCast(std::vector<rp3d::Ray*> a_vRays, bool a_bDeleteRays /*=false*/) const
{
	//List of hits to return
	std::vector<RayCastHitsInfo*> vAllRayHits;

	//Loop through all of the rays and cast them, if they have a collision
	//add them to the ray hits otherwise they are not usefull delete them.
	for(int rayIndex = 0; rayIndex < a_vRays.size(); ++rayIndex)
	{
		//Cast and check for hit
		rp3d::Ray* pCurrentRay = a_vRays[rayIndex];
		RayCastHitsInfo* pRayHit = RayCast(pCurrentRay);
		if(!pRayHit->m_vRayCastHits.empty())
		{
			vAllRayHits.push_back(pRayHit);
		}
		else {
			//Ray hit nothing delete it
			delete pRayHit;
		}
		

		//If we should delete the ray itself then delete it
		if(a_bDeleteRays)
		{
			delete pCurrentRay;
		}
	}

	//Return the vector of all the hits we have collected
	return vAllRayHits;
	
}


#pragma region Collision Callback Info

/// <summary>
/// Function to implement required virtual function in CollisionCallbackInfo class
/// Triggered whenever we detect a collision between 2 collision bodies.
/// </summary>
/// <param name="a_pCollisionCallbackInfo"></param>
void CollisionInfo::notifyContact(const CollisionCallbackInfo& a_pCollisionCallbackInfo)
{
	//Get the entities for each of the bodies involved in the collision if they are both
	//not nullptrs (as we need to have 2 bodies in a collision for it to be valid)
	//then add them to the list
	Entity* collisionEntity1 = ColliderComponent::GetEntityFromCollisionBody(a_pCollisionCallbackInfo.body1);
	Entity* collisionEntity2 = ColliderComponent::GetEntityFromCollisionBody(a_pCollisionCallbackInfo.body2);

	if(collisionEntity1 && collisionEntity2)
	{
		m_aCollisionEntities.push_back(collisionEntity1);
		m_aCollisionEntities.push_back(collisionEntity2);
	}
}

#pragma endregion 

#pragma region Raycast Callback Info
//Function called when the ray cast hits a collider in the world
rp3d::decimal RayCastHitsInfo::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
	//Create a raycast hit and fill it with our info
	RayCastHit* hit = new RayCastHit();
	hit->m_pHitEntity = ColliderComponent::GetEntityFromCollisionBody(info.body);
	hit->m_v3HitPoint = glm::vec3(info.worldPoint.x, info.worldPoint.y, info.worldPoint.z);
	hit->m_v3HitNormal = glm::vec3(info.worldNormal.x, info.worldNormal.y, info.worldNormal.z);
	hit->m_fHitFraction = info.hitFraction;

	//Add to hits list
	m_vRayCastHits.push_back(hit);

	// Return a decimal of 1.0 to gather all hits 
	return reactphysics3d::decimal(1.f);
}


#pragma endregion 