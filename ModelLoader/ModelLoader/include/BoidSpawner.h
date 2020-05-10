#ifndef __BOID_SPAWNER_H__
#define __BOID_SPAWNER_H__
#include "Singleton.h"

//C++ Includes
#include <vector>
#include <map>

//React Physics incluides
#include "ReactPhysics3D/reactphysics3d.h"

//Project Includes
#include "DoubleLinkedList.h"

//Forward Declares
class Model;

class BoidSpawner : public Singleton<BoidSpawner>
{
	friend class Singleton<BoidSpawner>;
public:
	void SpawnBoid();
	void SpawnBoids(unsigned int a_iCount);

	void DestroyBoid(Entity* a_pEntity);
	
	void UnloadAllModels();
private:
	BoidSpawner();
	~BoidSpawner();

	void LoadAllModels();

	//Linked list of all of the boids
	DoubleLinkedList<Entity> m_lpeActiveEntities;
	
	//Number of boids
	unsigned int m_iBoidCount;

	//Collision world to pass to boids
	rp3d::CollisionWorld* m_pBoidCollisionWorld;


	//All loaded models and number to load
	const int m_iModelCount = 15;
	std::vector<Model*> m_vpLoadedModels;
};

#endif //!__BOID_SPAWNER_H__