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
	//be removed when you destroy the body." - Section 10.7 (https://www.reactphysics3d.com/usermanual.html#x1-3800010)
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

	//TODO Cleanup
	//Test for collisions with every entity
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

		ColliderComponent* pTargetCollider = static_cast<ColliderComponent*>(pTarget->GetComponent(COMPONENT_TYPE::COLLIDER)); //TODO Make this Nicer

		//Test this bodies AABB against the currrent targets aabb
		if (m_pCollisionBody->testAABBOverlap(pTargetCollider->m_pCollisionBody->getAABB())) {
			int i = 0;
		}

	}
}

void ColliderComponent::Draw(Shader* a_pShader)
{
	//Blank
}

rp3d::Transform ColliderComponent::GetPhysicsTransform(TransformComponent* a_pTransform)
{
	//Ealry out, returning a default 
	//transform if we were not given a valid transform
	if (a_pTransform == nullptr) {
		return rp3d::Transform::identity();
	}

	//Convert Position Component
	glm::vec3 v3TransformPos = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);
	rp3d::Vector3 v3PosVector = rp3d::Vector3(v3TransformPos.x, v3TransformPos.y, v3TransformPos.z);

	//Convert Rotation Component
	glm::vec3 v3Right = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	glm::vec3 v3Up = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::UP_VECTOR);
	glm::vec3 v3Forward = a_pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	rp3d::Matrix3x3 m3RotVector = rp3d::Matrix3x3(v3Right.x,	v3Right.y,		v3Right.z,
													 v3Up.x,	v3Up.y,			v3Up.z,
												v3Forward.x,	v3Forward.y,	 v3Up.z);


	return rp3d::Transform(v3PosVector, m3RotVector);
}
