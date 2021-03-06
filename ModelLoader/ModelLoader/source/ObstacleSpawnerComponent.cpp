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

//Declare Statics - Spawner Defaults
const float ObstacleSpawnerComponent::sc_fDefaultSpawnDistance = 3.0f;
const float ObstacleSpawnerComponent::sc_fDefaultObstacleRadius = 1.f;
const unsigned int ObstacleSpawnerComponent::sc_fObstacleResolution = 10u;

/// <summary>
/// Create the obstacle spawner component
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
ObstacleSpawnerComponent::ObstacleSpawnerComponent(Entity* a_pOwner) :
	PARENT(a_pOwner),
	m_bSpawnBtnThisFrame(false),
	m_bSpawnBtnLastFrame(false),
	m_fObstacleRadius(sc_fDefaultObstacleRadius),
	m_fSpawnDistance(sc_fDefaultSpawnDistance)
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
		Gizmos::addSphere(GetObstacleSpawnPos(), sc_fObstacleResolution, sc_fObstacleResolution, m_fObstacleRadius, m_v4ObstacleColour);
	}
}

/// <summary>
/// Spawns an obstacle at a given position
/// </summary>
/// <param name="a_v3Position">Position to spawn at</param>
void ObstacleSpawnerComponent::SpawnObstacle(const glm::vec3 a_v3Position) const
{
	//Create an Entity
	Entity* pObstacleEntity = new Entity();
	pObstacleEntity->SetEntityType(ENTITY_TYPE::ENTITY_TYPE_OBSTACLE);
	
	//Create and apply transforms
	TransformComponent* pObstacleTransform = new TransformComponent(pObstacleEntity);
	pObstacleTransform->SetEntityMatrixRow(MATRIX_ROW::POSITION_VECTOR, a_v3Position);
	pObstacleEntity->AddComponent(pObstacleTransform);

	//Add Sphere - so we have a visual repreentation
	SpherePrimitiveComponent* pObstacleSphere = new SpherePrimitiveComponent(pObstacleEntity);
	pObstacleSphere->SetDimensions(m_fObstacleRadius);
	pObstacleEntity->AddComponent(pObstacleSphere);

	//Add collider - so we entities avoid this
	ColliderComponent* pObstacleCollider = new ColliderComponent(pObstacleEntity, Scene::GetInstance()->GetCollisionWorld());
	pObstacleCollider->AddSphereCollider(m_fObstacleRadius, glm::vec3(0));
	pObstacleEntity->AddComponent(pObstacleCollider);

}


/// <summary>
/// Gets the position that we will spawn the obstacle
/// </summary>
/// <returns></returns>
glm::vec3 ObstacleSpawnerComponent::GetObstacleSpawnPos() const
{
	//Initialize spawn pos to 0, so we can just return this if we fail
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

/// <summary>
/// Get the name of the component to display in text
/// </summary>
/// <returns></returns>
const char* ObstacleSpawnerComponent::GetComponentName() const
{
	return m_szName;
}