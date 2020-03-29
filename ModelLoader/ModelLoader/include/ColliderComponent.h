#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

//Project Includes
#include "Component.h"
#include "TransformComponent.h"

#include <ReactPhysics3D/reactphysics3d.h>

class ColliderComponent : public Component {
public:
	ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld);
	~ColliderComponent();


	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

private:
	rp3d::CollisionBody* m_pCollisionBody;
	rp3d::CollisionWorld* m_pCollisionWorld;

	const float m_fColliderRadius = .25f;

	//Convert from our transform to the rp3d transform
	rp3d::Transform GetPhysicsTransform(TransformComponent* a_pTransform);

	//Collision Shapes - physical shape that we use and the proxy shape,
	//used by the collision system
	rp3d::SphereShape* m_pCollisionShape; //Physical properties of the collision shape
	rp3d::ProxyShape* m_pProxyShape; //Proxy shape is the collision shape with mass and transform infomation
};

#endif // !__COLLIDER_COMPONENT_H__