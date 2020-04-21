#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

//Project Includes
#include "Component.h"

//Lib Includes
#include <glm/ext.hpp>

//todo this needs a big cleanup

enum MATRIX_ROW 
{
	RIGHT_VECTOR,
	UP_VECTOR,
	FORWARD_VECTOR,
	POSTION_VECTOR
};

class TransformComponent : public Component
{
public:
	TransformComponent(Entity* a_pOwner);
	~TransformComponent();

	virtual void Update(float a_fDeltaTime) {};
	virtual void Draw(Shader* a_pShader) {};

	void Orthogonalize();

	//Get and set Matrix Row
	const glm::mat4& GetEntityMatrix() const;
	void SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec);
	glm::vec3 GetEntityMatrixRow(MATRIX_ROW a_eRow);
	glm::vec3 GetCurrentPosition();

private:

	/*
	Constants for default values (i.e those that make up and
	identity matrix)
	*/
	const glm::vec3 mc_v3WorldUp = glm::vec3(0.f, 1.f, 0.f);
	const glm::vec3 mc_v3WorldRight = glm::vec3(1.f, 0.f, 0.f);
	
	glm::mat4 m_m4EntityMatrix;
};

#endif // ! __TRANSFORM_COMPONENT_H__
