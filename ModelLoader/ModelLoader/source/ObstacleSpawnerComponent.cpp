#include "ObstacleSpawnerComponent.h"

//Project Includes
#include "Gizmos.h"
#include "Entity.h"
//Components
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"
#include "ColliderComponent.h"
#include "Scene.h"

//Typedefs
typedef Component PARENT;

//Defaults
constexpr float mc_fDefaultSpawnDistance = 3.0f;
constexpr float mc_fDefaultObstacleRadius = 0.25f;
constexpr unsigned int mc_fObstacleResolution = 20u;

ObstacleSpawnerComponent::ObstacleSpawnerComponent(Entity* a_pOwner):
	PARENT(a_pOwner),
	m_bSpawnBtnThisFrame(false),
	m_bSpawnBtnLastFrame(false),
	m_fObstacleRadius(mc_fDefaultObstacleRadius),
	m_fSpawnDistance(mc_fDefaultSpawnDistance)
{
}

/// <summary>
/// Update the spawner component
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
void ObstacleSpawnerComponent::Update(float a_fDeltaTime)
{
	//Get our active window
	GLFWwindow* pWindow = glfwGetCurrentContext();
	if(!pWindow)
	{
		return;
	}

	//Get the button status
	m_bSpawnBtnThisFrame = (glfwGetMouseButton(pWindow, m_iSpawnBtn) == GLFW_PRESS);

	//If we have released the button this frame then spawn the obsticle
	if(m_bSpawnBtnLastFrame && !m_bSpawnBtnThisFrame)
	{
		SpawnObstacle(GetObstacleSpawnPos());
	}
	
	//End of frame - set button state from this frame
	m_bSpawnBtnLastFrame = m_bSpawnBtnThisFrame;
}

/// <summary>
/// Draw the preview of where the obsticle will go
/// </summary>
/// <param name="a_pShader"></param>
void ObstacleSpawnerComponent::Draw(Shader* a_pShader)
{
	//If the button was pressed this frame then draw a preview of where we are
	//going to spawn it
	if(m_bSpawnBtnLastFrame)
	{
		Gizmos::addSphere(GetObstacleSpawnPos(), mc_fObstacleResolution, mc_fObstacleResolution, m_fObstacleRadius, m_v4ObstacleColour);
	}
}

/// <summary>
/// Spawns an obstacle at a given position
/// </summary>
/// <param name="a_v3Position">Position to spawn at</param>
void ObstacleSpawnerComponent::SpawnObstacle(glm::vec3 a_v3Position)
{
	//Create an Entity
	Entity* pObstacleEntity = new Entity();
	pObstacleEntity->SetEntityType(ENTITY_TYPE::ENTITY_TYPE_OBSTACLE);
	
	//Create components - we need a transform, primative and collider
	TransformComponent* pObstacleTransform = new TransformComponent(pObstacleEntity);
	SpherePrimitiveComponent* pObstacleSphere = new SpherePrimitiveComponent(pObstacleEntity);
	ColliderComponent* pObstacleCollider = new ColliderComponent(pObstacleEntity, Scene::GetInstance()->GetCollisionWorld());

	//Set dimentions of collider and sphere so that they match
	pObstacleSphere->SetDimensions(m_fObstacleRadius);
	pObstacleCollider->AddSphereCollider(m_fObstacleRadius, glm::vec3(0));
	//Set position so it is correct
	pObstacleTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, a_v3Position);

	//Add all of our components to the entity
	pObstacleEntity->AddComponent(pObstacleTransform);
	pObstacleEntity->AddComponent(pObstacleCollider);
	pObstacleEntity->AddComponent(pObstacleSphere);
}


/// <summary>
/// Gets the position that we will spawn the obstacle
/// </summary>
/// <returns></returns>
glm::vec3 ObstacleSpawnerComponent::GetObstacleSpawnPos() const
{
	//Initalise spawn pos to 0, so we can just return this if we fail
	glm::vec3 v3SpawnPos(0.f);
	
	if(!m_pOwnerEntity)
	{
		return v3SpawnPos;
	}
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if(!pTransform)
	{
		return v3SpawnPos;
	}

	//We should spawn in in front of the camera (or other object)
	//so add it's forward to our position and mutiply by the distance we want to spawn at
	const glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();
	const glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	v3SpawnPos = v3CurrentPos + (v3Forward * m_fSpawnDistance);
	
	return v3SpawnPos;
}
