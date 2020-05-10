#include "RaycastComponent.h"

//React Physics Include
#include <ReactPhysics3D/reactphysics3d.h>

#include "ColliderComponent.h"

//Typedefs
typedef Component PARENT;

/// <summary>
/// Create a Raycaster Component
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
/// <param name="a_pCollisionWorld">Collision World to test raycasts in</param>
RaycastComponent::RaycastComponent(Entity* a_pOwner, reactphysics3d::CollisionWorld* a_pCollisionWorld) :
	PARENT(a_pOwner),
	m_pCollisionWorld(a_pCollisionWorld)
{
}

/// <summary>
/// Ray casts between 2 points in the world
/// </summary>
/// <param name="a_v3StartPoint">World Start Point</param>
/// <param name="a_v3EndPoint">World End Point</param>
/// <returns>Infomation about all of the objects that the raycast has hit</returns>
RayCastHitsInfo* RaycastComponent::RayCast(const glm::vec3 a_v3StartPoint, const glm::vec3 a_v3EndPoint) const
{
	//Create a ray from the given start and end point
	const rp3d::Vector3 v3StartPoint(a_v3StartPoint.x, a_v3StartPoint.y, a_v3StartPoint.z);
	const rp3d::Vector3 v3EndPoint(a_v3EndPoint.x, a_v3EndPoint.y, a_v3EndPoint.z);
	rp3d::Ray raycastRay(v3StartPoint, v3EndPoint);

	//Call function that takes ray as parameter and return the result of that
	return RayCast(&raycastRay);
}

/// <summary>
/// Ray casts between 2 points in the world
/// </summary>
/// <param name="a_ray">Ray to cast</param>
/// <returns>Infomation about all of the objects that the raycast has hit</returns>
RayCastHitsInfo* RaycastComponent::RayCast(rp3d::Ray* a_ray) const
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
std::vector<RayCastHitsInfo*> RaycastComponent::MutiRayCast(std::vector<rp3d::Ray*> a_vRays, bool a_bDeleteRays /*=false*/) const
{
	//List of hits to return
	std::vector<RayCastHitsInfo*> vAllRayHits;

	//Loop through all of the rays and cast them, if they have a collision
	//add them to the ray hits otherwise they are not usefull delete them.
	for (int rayIndex = 0; rayIndex < a_vRays.size(); ++rayIndex)
	{
		//Cast and check for hit
		rp3d::Ray* pCurrentRay = a_vRays[rayIndex];
		RayCastHitsInfo* pRayHit = RayCast(pCurrentRay);
		if (!pRayHit->m_vRayCastHits.empty())
		{
			vAllRayHits.push_back(pRayHit);
		}
		else {
			//Ray hit nothing delete it
			delete pRayHit;
		}


		//If we should delete the ray itself then delete it
		if (a_bDeleteRays)
		{
			delete pCurrentRay;
		}
	}

	//Return the vector of all the hits we have collected
	return vAllRayHits;

}

#pragma region Raycast Callback Info

//Function called when the ray cast hits a collider in the world
rp3d::decimal RayCastHitsInfo::notifyRaycastHit(const rp3d::RaycastInfo& a_pRaycastInfo)
{
	//Create a raycast hit and fill it with our info
	RayCastHit* hit = new RayCastHit();
	hit->m_pHitEntity = ColliderComponent::GetEntityFromCollisionBody(a_pRaycastInfo.body);
	hit->m_v3HitPoint = glm::vec3(a_pRaycastInfo.worldPoint.x, a_pRaycastInfo.worldPoint.y, a_pRaycastInfo.worldPoint.z);
	hit->m_v3HitNormal = glm::vec3(a_pRaycastInfo.worldNormal.x, a_pRaycastInfo.worldNormal.y, a_pRaycastInfo.worldNormal.z);
	hit->m_fHitFraction = a_pRaycastInfo.hitFraction;

	//Add to hits list
	m_vRayCastHits.push_back(hit);

	// Return a decimal of 1.0 to gather all hits 
	return reactphysics3d::decimal(1.f);
}


#pragma endregion 

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns></returns>
const char* RaycastComponent::GetComponentName() const
{
	return m_szName;
}