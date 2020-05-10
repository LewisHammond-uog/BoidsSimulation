#ifndef __BOX_PRIMITIVE_COMPONENT_H__
#define __BOX_PRIMITIVE_COMPONENT_H__

//Project Includes
#include <glm/glm.hpp>

#include "PrimitiveComponent.h"

/// <summary>
/// Component to draw a box in the world
/// </summary>
class BoxPrimitiveComponent : public PrimitiveComponent
{
public:
	explicit BoxPrimitiveComponent(Entity* a_pOwner, glm::vec3 a_v3BoxDimensions = glm::vec3(1, 1, 1));
	~BoxPrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(glm::vec3 a_v3NewDimensions);
	
	//Update/Draw Functions
	void Update(float a_fDeltaTime) override = 0;
	void Draw(Shader* a_pShader) override;

	//Get text name of the component
	const char* GetComponentName() const override;
private:
	//Dimentions of the box
	glm::vec3 m_v3BoxDimensions;

	const char* m_szName = "Primitive (Box)";
};

#endif //!__BOX_PRIMITIVE_COMPONENT_H__