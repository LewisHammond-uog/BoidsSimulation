#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

//c++ includes
#include <vector>

//Project Includes
#include <iostream>

#include "Component.h"
#include "TransformComponent.h"
#include <ReactPhysics3D/reactphysics3d.h>

class CollisionInfo : public rp3d::CollisionCallback {
public:
	virtual void notifyContact(const CollisionCallbackInfo& a_pCollisionCallbackInfo);
private:
	std::vector<Entity*> m_aCollisionEntities;
	bool m_bCollisionIsValid = false; 
};

class RaycastCallbackInfo : public rp3d::RaycastCallback
{
public:

	virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) {

		// Display the world hit point coordinates 
		std::cout << "Hit point : " <<
			info.worldPoint.x <<
			info.worldPoint.y <<
			info.worldPoint.z <<
			std::endl;

		// Return a fraction of 1.0 to gather all hits 
		return reactphysics3d::decimal(1.0);
	}
};

class ColliderComponent : public Component {
	friend class CollisionInfo;
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
	std::vector<CollisionInfo> GetCollisionInfo();
	CollisionInfo GetCollisionInfo(ColliderComponent* a_pOtherCollider) const;

	//TODO - Fix this? Never hits anything
	//Functions to do raycasting
	rp3d::RaycastInfo* RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint);
	rp3d::RaycastInfo* RayCast(rp3d::Ray* a_Ray);

private:

	//Convert from our transform to the rp3d transform
	static rp3d::Transform GetPhysicsTransform(TransformComponent* a_pTransform);
	//Checks if a collision check between 2 colliders is valid - performs null checks on
	//colliding components
	bool IsCollisionCheckValid(ColliderComponent* a_pOtherCollider) const;
	
	rp3d::CollisionBody* m_pCollisionBody; //Pointer to the rp3d collision body that is used in the physics system
	rp3d::CollisionWorld* m_pCollisionWorld; //Pointer to the physics world that this object is using

	//Collision Shapes - physical shape that we use and the proxy shape,
	//used by the collision system
	std::vector<rp3d::CollisionShape*> m_apCollisionShapes; //List of physical shapes used
	std::vector<rp3d::ProxyShape*> m_apProxyShapes; //List of proxy shapes. Proxy shape is the collision shape with mass and transform info

};

#endif // !__COLLIDER_COMPONENT_H__