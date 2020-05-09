#ifndef __BOID_SPAWNER_H__
#define __BOID_SPAWNER_H__
#include "Singleton.h"

//C++ Includes
#include <vector>
#include <map>

//React Physics incluides
#include "ReactPhysics3D/reactphysics3d.h"

//Forward Declares
class Model;

class BoidSpawner : public Singleton<BoidSpawner>
{
	friend class Singleton<BoidSpawner>;
public:
	void SpawnBoid();
	void SpawnBoids(unsigned int a_iCount);

	void DestroyBoid();
	void DestroyBoid(unsigned int a_id);
	
	void UnloadAllModels();
private:
	BoidSpawner();
	~BoidSpawner() = default;

	void LoadAllModels();

	//List of all of the boids that we have spawned
	static std::map<const unsigned int, Entity*> s_xBoidMap;
	
	//Number of boids
	unsigned int m_iBoidCount;

	//Collision world to pass to boids
	rp3d::CollisionWorld* m_pBoidCollisionWorld;


	//All loaded models and number to load
	const int m_iModelCount = 15;
	std::vector<Model*> m_vpLoadedModels;
};

#endif //!__BOID_SPAWNER_H__