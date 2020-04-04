#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

//c++ includes
#include <vector>

//Project Includes
#include "Component.h"
#include "TransformComponent.h"
#include <ReactPhysics3D/reactphysics3d.h>

class CollisionInfo : public rp3d::CollisionCallback {
public:
	virtual void notifyContact(const CollisionCallbackInfo& a_collisionCallbackInfo);
private:
	std::vector<Entity*> m_aCollisionEntities;
	bool m_bCollisionIsValid = false; 
};

class ColliderComponent : public Component {
	friend class CollisionInfo;
public:
	ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld);
	~ColliderComponent();


	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	//Functions to check for a collision
	bool IsColliding(bool a_bUseAABB);
	bool IsColliding(ColliderComponent* a_pOtherCollider, bool a_bUseAABB) const;

	//Functions to get collision info from a collision
	std::vector<CollisionInfo> GetCollisionInfo();
	CollisionInfo GetCollisionInfo(ColliderComponent* a_pOtherCollider) const;

	//TODO - Fix this? Never hits anything
	//Functions to do raycasting
	//rp3d::RaycastInfo* RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint);
	//rp3d::RaycastInfo* RayCast(rp3d::Ray* a_Ray);

private:
	rp3d::CollisionBody* m_pCollisionBody;
	rp3d::CollisionWorld* m_pCollisionWorld;
	
	const float m_fColliderRadius = .25f;

	//Collision Shapes - physical shape that we use and the proxy shape,
	//used by the collision system
	rp3d::SphereShape* m_pCollisionShape; //Physical properties of the collision shape
	rp3d::ProxyShape* m_pProxyShape; //Proxy shape is the collision shape with mass and transform infomation

	//Convert from our transform to the rp3d transform
	static rp3d::Transform GetPhysicsTransform(TransformComponent* a_pTransform);
	//Checks if a collision check between 2 colliders is valid - performs null checks on
	//colliding components
	bool IsCollisionCheckValid(ColliderComponent* a_pOtherCollider) const;


};

#endif // !__COLLIDER_COMPONENT_H__