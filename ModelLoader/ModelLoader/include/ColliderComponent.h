#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

//C++ includes
#include <vector>

//rp3d Includes
#include <ReactPhysics3D/reactphysics3d.h>

//Project Includes
#include "Component.h"
#include "TransformComponent.h"


//Forward Declare
class CollisionInfo;
class RayCastHitsInfo;
struct RayCastHit;

/// <summary>
/// Component used for detecting collisions between 2 entities in the world
/// </summary>
class ColliderComponent : public Component {
	friend class CollisionInfo;
	friend class RayCastHitsInfo;
public:
	ColliderComponent(Entity* a_pOwner, rp3d::CollisionWorld* a_pCollisionWorld);
	~ColliderComponent();


	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	//Functions to add collider shapes
	void AddBoxCollider(glm::vec3 a_v3BoxSize, glm::vec3 a_v3Offset);
	void AddSphereCollider(float a_fSphereSize, glm::vec3 a_v3Offset);

	//Functions to check for a collision
	bool IsColliding(bool a_bUseAABB) const;
	bool IsColliding(ColliderComponent* a_pOtherCollider, bool a_bUseAABB) const;

	//Functions to get collision info from a collision
	std::vector<CollisionInfo*> GetCollisionInfo() const;
	CollisionInfo* GetCollisionInfo(ColliderComponent* a_pOtherCollider) const;

	//Functions to do raycasting
	RayCastHitsInfo* RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint) const;
	RayCastHitsInfo* RayCast(rp3d::Ray* a_ray) const;
	std::vector<RayCastHitsInfo*> MutiRayCast(std::vector<rp3d::Ray*> a_vRays, bool a_bDeleteRays =false) const;

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

	//Constants for debug collider drawing
	const glm::vec4 mc_v4ColliderDrawCol = glm::vec4(0, 0, 0, 1);
	const unsigned int mc_iColliderDrawRes = 10u;
	
};

/// <summary>
/// Class which contains info about a collision between
/// a number of entities
/// </summary>
class CollisionInfo final : public rp3d::CollisionCallback {
public:
	//Default Constructor
	CollisionInfo() = default;
	//Default Desctructor
	~CollisionInfo() = default;
	
	void notifyContact(const CollisionCallbackInfo& a_pCollisionCallbackInfo) override;
private:
	std::vector<Entity*> m_aCollisionEntities;
};


/// <summary>
/// Class which represents a single hit of a raycast
/// Includes the hit body and the point that the hit occoured at
/// </summary>
struct RayCastHit
{
	Entity* m_pHitEntity; //Entity that we have hit
	glm::vec3 m_v3HitPoint; //Point in space that we hit the entity
	glm::vec3 m_v3HitNormal; //The normal of the point that we hit
	float m_fHitFraction = 1.f; //Fraction of the distance that we hit the object in the ray cast (range 0-1)
};



/// <summary>
/// Class which handles all of the objects that a raycast has hit.
/// Contains info about the entity, hit point, hit normal and hit fraction
/// All hits are gathered in the order that they occoured
/// </summary>
class RayCastHitsInfo final : public rp3d::RaycastCallback
{
public:

	//Default Constructor
	RayCastHitsInfo() = default;
	
	//Destructor, removes all raycast hits
	~RayCastHitsInfo()
	{
		if (!m_vRayCastHits.empty()) {
			for (int i = 0; i < m_vRayCastHits.size(); ++i)
			{
				delete m_vRayCastHits[i];
			}
			m_vRayCastHits.clear();
		}
	}

	rp3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& raycastInfo) override;
	
	//List of hits from this raycast
	std::vector<RayCastHit*> m_vRayCastHits;
	
};

#endif // !__COLLIDER_COMPONENT_H__