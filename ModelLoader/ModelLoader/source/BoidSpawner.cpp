#include "BoidSpawner.h"

//Learn OpenGL includes
#include "learnopengl/model.h"

//Project Includes
#include "Scene.h"
#include "MathsUtils.h"
//Colliders
#include "ColliderComponent.h"
#include "BrainComponent.h"
#include "Entity.h"
#include "ModelComponent.h"
#include "RaycastComponent.h"

//Typedefs
typedef std::pair<const unsigned int, Entity*> BoidPair;

//Construct the boid spawner
BoidSpawner::BoidSpawner() :
	m_iBoidCount(0u)
{
	//bug - This is not changed on scene restart and thus passes the wrong collision world to the boids
	//Get the collision world
	m_pBoidCollisionWorld = Scene::GetInstance()->GetCollisionWorld();

	//Load all of the models 
	LoadAllModels();
}

//Destructor
BoidSpawner::~BoidSpawner()
{
	//Remove all of the boids from the linked list
	//entity cleanup is dealt with by the scene
	m_lpActiveEntities.Clear();
}


/// <summary>
/// Spawns a single boid
/// </summary>
void BoidSpawner::SpawnBoid()
{
	Entity* pEntity = new Entity();
	pEntity->SetEntityType(ENTITY_TYPE::ENTITY_TYPE_BOID);

	//Transform Component
	TransformComponent* pTransform = new TransformComponent(pEntity);
	pTransform->SetEntityMatrixRow(MATRIX_ROW::POSITION_VECTOR, glm::vec3(MathsUtils::RandomRange(-5.0f, 5.0f),
		MathsUtils::RandomRange(-5.0f, 5.0f),
		MathsUtils::RandomRange(-5.0f, 5.0f)));
	pEntity->AddComponent(pTransform);

	//Model Component
	ModelComponent* pModel = new ModelComponent(pEntity);
	pModel->ChooseRandomModel(m_vpLoadedModels);
	pModel->SetScale(0.02f);
	pEntity->AddComponent(pModel);

	//Brain Component
	BrainComponent* pBrain = new BrainComponent(pEntity);
	pEntity->AddComponent(pBrain);

	//Collider Component
	ColliderComponent* pCollider = new ColliderComponent(pEntity, m_pBoidCollisionWorld);
	pCollider->AddSphereCollider(0.25f, glm::vec3(0.0f));
	pEntity->AddComponent(pCollider);

	//Raycast Component
	RaycastComponent* pRayCaster = new RaycastComponent(pEntity, m_pBoidCollisionWorld);
	pEntity->AddComponent(pRayCaster);

	//Add to linked list
	m_lpActiveEntities.Push(pEntity);

	++m_iBoidCount;
}

/// <summary>
/// Spawn a given number of boids
/// </summary>
/// <param name="a_iCount">Number of boids to spawn</param>
void BoidSpawner::SpawnBoids(const unsigned int a_iCount)
{
	for (int i = 0; i < a_iCount; ++i)
	{
		SpawnBoid();
	}
}

/// <summary>
/// Adjusts the boid count based on a target count of boids.
/// Compares the target to the current count and spawns or
/// destroy boids to get to this target
/// </summary>
/// <param name="a_iTargetCount"></param>
void BoidSpawner::AdjustBoidCount(const unsigned int a_iTargetCount)
{
	const int iCountDiff = a_iTargetCount - m_iBoidCount;

	//If we have too few boids spawn them, otherwise destroy
	if(iCountDiff > 0)
	{
		SpawnBoids(iCountDiff);
	}else if(iCountDiff < 0)
	{
		DestroyBoids(abs(iCountDiff));
	}
}

/// <summary>
/// Destroy a boid
/// </summary>
/// <param name="a_pEntity">Boid to destroy</param>
void BoidSpawner::DestroyBoid(Entity* a_pEntity)
{
	//Remove the entity from the linked list
	m_lpActiveEntities.Pop(a_pEntity);
	
	//Delete Entity
	delete a_pEntity;
	--m_iBoidCount;
}

/// <summary>
/// Destroy a given number of boids
/// </summary>
/// <param name="a_iCount"></param>
void BoidSpawner::DestroyBoids(const unsigned a_iCount)
{
	//Destroy the given number of boids by destroying the
	//boid at the start of the linked list
	for(int i = 0; i < a_iCount; ++i)
	{
		//Get boid at the head of the linked list
		Entity* currentBoid = m_lpActiveEntities.Search(0);

		//If the boid is a nullptr then the list is empty and thus we cannot destory
		//anymore boids
		if(currentBoid == nullptr)
		{
			return;
		}

		//Remove from Linked List and Destroy
		m_lpActiveEntities.Pop(currentBoid);
		DestroyBoid(currentBoid);
	}
}

/// <summary>
/// Load all of the models used by the program
/// </summary>
void BoidSpawner::LoadAllModels()
{
	/* Model Files are stored in the format
	 * Fish01, Fish02 etc. so we just loop through
	 * with the same prefix to load all of the models
	 */
	const std::string modelFilePrefix = "models/fish/Fish0";
	const std::string modelFileSuffix = ".obj";

	//Loop until we have loaded all models
	for (int i = 1; i <= m_iModelCount; ++i)
	{
		//Generate file name and load model
		const std::string modelFileName = modelFilePrefix + std::to_string(i) + modelFileSuffix;
		Model* newModel = new Model(modelFileName);

		if (newModel != nullptr) {
			//Add to our loaded models
			m_vpLoadedModels.push_back(newModel);
		}
	}

}

/// <summary>
/// Unload all of the models
/// </summary>
void BoidSpawner::UnloadAllModels()
{
	for (int i = 0; i < m_vpLoadedModels.size(); ++i)
	{
		delete m_vpLoadedModels[i];
	}
	m_vpLoadedModels.clear();
}
