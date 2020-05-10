#ifndef __RAYCAST_COMPONENT_H__
#define __RAYCAST_COMPONENT_H__

//C++ Includes
#include <vector>

//rp3d Includes
#include "ReactPhysics3D/reactphysics3d.h"

//Project Includes
#include "Component.h"

//Forward Declares
class RayCastHitsInfo;


/// <summary>
/// Component to Raycast in the world
/// </summary>
class RaycastComponent : public Component
{
public:
	RaycastComponent(Entity* a_pOwner, reactphysics3d::CollisionWorld* a_pCollisionWorld);
	~RaycastComponent() = default;

	//Component Functions
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};
	
	//Functions to do raycasting
	RayCastHitsInfo* RayCast(glm::vec3 a_v3StartPoint, glm::vec3 a_v3EndPoint) const;
	RayCastHitsInfo* RayCast(rp3d::Ray* a_ray) const;
	std::vector<RayCastHitsInfo*> MutiRayCast(std::vector<rp3d::Ray*> a_vRays, bool a_bDeleteRays = false) const;

	//Get text name of the component
	const char* GetComponentName() const override;
	
private:
	rp3d::CollisionWorld* m_pCollisionWorld; //Pointer to the physics world that this object is using
	const char* m_szName = "Raycast";
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
	float m_fHitFraction; //Fraction of the distance that we hit the object in the ray cast (range 0-1)
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
			for (unsigned int i = 0; i < m_vRayCastHits.size(); ++i)
			{
				delete m_vRayCastHits[i];
			}
			m_vRayCastHits.clear();
		}
	}

	rp3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& a_pRaycastInfo) override;

	//List of hits from this raycast
	std::vector<RayCastHit*> m_vRayCastHits;

};


#endif //!__RAYCAST_COMPOENENT_H__
