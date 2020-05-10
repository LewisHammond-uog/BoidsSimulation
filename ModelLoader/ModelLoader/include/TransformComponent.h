#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

//Project Includes
#include "Component.h"

//Lib Includes
#include <glm/ext.hpp>

enum class MATRIX_ROW 
{
	RIGHT_VECTOR,
	UP_VECTOR,
	FORWARD_VECTOR,
	POSITION_VECTOR
};

class TransformComponent : public Component
{
public:
	explicit TransformComponent(Entity* a_pOwner);
	~TransformComponent() = default;

	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};

	void Orthogonalize();

	//Get and set Matrix Rows
	const glm::mat4& GetEntityMatrix() const;
	void SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec);
	glm::vec3 GetEntityMatrixRow(MATRIX_ROW a_eRow);
	glm::vec3 GetCurrentPosition();

	//Get text name of the component
	const char* GetComponentName() const override;

private:

	/*
	Constants for default values (i.e those that make up and
	identity matrix)
	*/
	const glm::vec3 mc_v3WorldUp = glm::vec3(0.f, 1.f, 0.f);
	const glm::vec3 mc_v3WorldRight = glm::vec3(1.f, 0.f, 0.f);

	const char* m_szName = "Transform";
	
	glm::mat4 m_m4EntityMatrix;
};

#endif // ! __TRANSFORM_COMPONENT_H__
