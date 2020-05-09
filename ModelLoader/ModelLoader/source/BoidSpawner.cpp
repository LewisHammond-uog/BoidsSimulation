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

std::map<const unsigned int, Entity*> BoidSpawner::s_xBoidMap;

//Typedefs
typedef std::pair<const unsigned int, Entity*> BoidPair;

//Construct the boid spawner
BoidSpawner::BoidSpawner() :
	m_iBoidCount(0u)
{
	//Get the collision world
	m_pBoidCollisionWorld = Scene::GetInstance()->GetCollisionWorld();
	LoadAllModels();
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

	m_iBoidCount++;

	//Add boid to boid map
	s_xBoidMap.insert(BoidPair(m_iBoidCount, pEntity));
}

/// <summary>
/// Spawn a given number of boids
/// </summary>
/// <param name="a_iCount">Number of boids to spawn</param>
void BoidSpawner::SpawnBoids(const unsigned a_iCount)
{
	for(int i = 0; i < a_iCount; ++i)
	{
		SpawnBoid();
	}
}

/// <summary>
/// Destroys the last created boid
/// </summary>
void BoidSpawner::DestroyBoid()
{

}


/// <summary>
/// Destory a boid of a given ID
/// </summary>
/// <param name="a_id">ID of boid to destroy</param>
void BoidSpawner::DestroyBoid(unsigned a_id)
{
	//Check that boid ID exists then delete
	if(s_xBoidMap.find(a_id) != s_xBoidMap.end())
	{
		delete s_xBoidMap[a_id];
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
