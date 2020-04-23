#ifndef __OBSTACLE_SPAWNER_COMPONENT_H__
#define __OBSTACLE_SPAWNER_COMPONENT_H__

//GLFW Includes - for button mapping
#include <GLFW/glfw3.h>

//GLM Includes
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Component.h"


class ObstacleSpawnerComponent : public Component
{
public:
	ObstacleSpawnerComponent(Entity* a_pOwner);
	~ObstacleSpawnerComponent() ;

	//Update/Draw Functions
	virtual void Update(float a_fDeltaTime) override;
	virtual void Draw(Shader* a_pShader) override;

private:

	void SpawnObstacle(glm::vec3 a_v3Position);

	glm::vec3 GetObstacleSpawnPos() const;

	//Obsticle Color
	glm::vec4 m_v4ObstacleColour = glm::vec4(1.f, 0.65f, 0.f, 1.f);
	
	//States of the button presses this and last frame
	bool m_bSpawnBtnThisFrame;
	bool m_bSpawnBtnLastFrame;

	//Mouse Button used to spawn
	int m_iSpawnBtn = GLFW_MOUSE_BUTTON_2;

	float m_fSpawnDistance;
	float m_fObstacleRadius;


};

#endif //!__OBSTACLE_SPAWNER_COMPONENT_H__

