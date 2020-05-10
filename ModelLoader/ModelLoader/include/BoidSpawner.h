#ifndef __BOID_SPAWNER_H__
#define __BOID_SPAWNER_H__
#include "Singleton.h"

//C++ Includes
#include <vector>

//React Physics incluides
#include "ReactPhysics3D/reactphysics3d.h"

//Project Includes
#include "DoubleLinkedList.h"

//Forward Declares
class Model;

/// <summary>
/// Class to spawn boids within the world
/// </summary>
class BoidSpawner : public Singleton<BoidSpawner>
{
	friend class Singleton<BoidSpawner>;
public:
	void SpawnBoid();
	void SpawnBoids(unsigned int a_iCount);

	void AdjustBoidCount(unsigned int a_iTargetCount);
	
	void DestroyBoid(Entity* a_pEntity);
	void DestroyBoids(unsigned int a_iCount);

	Entity* GetBoidInfo(unsigned int a_iBoidPos);
	
	void SetCollisionWorld(rp3d::CollisionWorld* a_pCollisionWorld);
	
	void UnloadAllModels();
	
private:
	BoidSpawner();
	~BoidSpawner();

	void LoadAllModels();

	//Margin from the world edges to spawn
	const float m_fSpawnMargin = 1.0f;
	
	//Linked list of all of the boids
	DoubleLinkedList<Entity> m_lpActiveEntities;
	
	//Number of boids currently active
	unsigned int m_iBoidCount;

	//Collision world to pass to boids
	rp3d::CollisionWorld* m_pBoidCollisionWorld;

	//All loaded models and number to load
	const int m_iModelCount = 15;
	std::vector<Model*> m_vpLoadedModels;
};



#endif //!__BOID_SPAWNER_H__
