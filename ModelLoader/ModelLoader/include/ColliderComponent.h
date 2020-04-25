#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

//c++ includes
#include <vector>

//Project Includes
#include <iostream>

#include "Component.h"
#include "TransformComponent.h"
#include <ReactPhysics3D/reactphysics3d.h>

//Forward Declare
class CollisionInfo;
class RaycastCallbackInfo;

class ColliderComponent : public Component {
	friend class CollisionInfo;
	friend class RaycastCallbackInfo;
public:
	ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld);
	~ColliderComponent();


	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	//Functions to add collider shapes
	void AddBoxCollider(glm::vec3 a_v3BoxSize, glm::vec3 a_v3Offset);
	void AddSphereCollider(float a_fSphereSize, glm::vec3 a_v3Offset);

	//Functions to check for a collision
	bool IsColliding(bool a_bUseAABB);
	bool IsColliding(ColliderComponent* a_pOtherCollider, bool a_bUseAABB) const;

	//Functions to get collision info from a collision
	std::vector<CollisionInfo*> GetCollisionInfo();
	CollisionInfo* GetCollisionInfo(ColliderComponent* a_pOtherCollider) const;

	//Functions to do raycasting
	RaycastCallbackInfo RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint);
	RaycastCallbackInfo RayCast(rp3d::Ray* a_Ray);

private:
	
	//Checks if a collision check between 2 colliders is valid - performs null checks on
	//colliding components
	bool IsCollisionCheckValid(ColliderComponent* a_pOtherCollider) const;

	//Function to get a entity from a collision body
	static Entity* GetEntityFromCollisionBody(rp3d::CollisionBody* a_collisionBody);
	
	rp3d::CollisionBody* m_pCollisionBody; //Pointer to the rp3d collision body that is used in the physics system
	rp3d::CollisionWorld* m_pCollisionWorld; //Pointer to the physics world that this object is using

	//Collision Shapes - physical shape that we use and the proxy shape,
	//used by the collision system
	std::vector<rp3d::CollisionShape*> m_apCollisionShapes; //List of physical shapes used
	std::vector<rp3d::ProxyShape*> m_apProxyShapes; //List of proxy shapes. Proxy shape is the collision shape with mass and transform info

};


//todo move
//todo make these names better
class CollisionInfo : public rp3d::CollisionCallback {
public:
	virtual void notifyContact(const CollisionCallbackInfo& a_pCollisionCallbackInfo);
private:
	std::vector<Entity*> m_aCollisionEntities;
};

//todo move
//todo make these names better
/// <summary>
/// Class which represents a hit of a raycast
/// Includes the hit body and the point that the hit occoured at
/// </summary>
struct RayCastHit
{
	Entity* m_pHitEntity; //Entity that we have hit
	glm::vec3 m_v3HitPoint; //Point in space that we hit the entity
	glm::vec3 m_v3HitNormal; //The normal of the point that we hit
	float m_fHitFraction; //Fraction of the distance that we hit the object in the ray cast (range 0-1)
};

//todo move
//todo make these names better
class RaycastCallbackInfo : public rp3d::RaycastCallback
{
public:

	//Function called when the ray cast hits a collider in the world
	virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) {

		//Create a raycast hit and fill it with our info
		RayCastHit* hit = new RayCastHit();
		hit->m_pHitEntity = ColliderComponent::GetEntityFromCollisionBody(info.body);
		hit->m_v3HitPoint = glm::vec3(info.worldPoint.x, info.worldPoint.y, info.worldPoint.z);
		hit->m_v3HitNormal = glm::vec3(info.worldNormal.x, info.worldNormal.y, info.worldNormal.z);
		hit->m_fHitFraction = info.hitFraction;
		

		//Add to hits list
		m_vRayCastHits.push_back(hit);

		// Return a fraction of 1.0 to gather all hits 
		return reactphysics3d::decimal(1.f);
	}

	//List of hits from this raycast
	std::vector<RayCastHit*> m_vRayCastHits;
};

#endif // !__COLLIDER_COMPONENT_H__