#ifndef __BOX_PRIMITIVE_COMPONENT_H__
#define __BOX_PRIMITIVE_COMPONENT_H__

//Project Includes
#include <glm/glm.hpp>

#include "PrimitiveComponent.h"

class BoxPrimitiveComponent : public PrimitiveComponent
{
public:
	BoxPrimitiveComponent(Entity* a_pOwner, glm::vec3 a_v3BoxDimensions = glm::vec3(1, 1, 1));
	~BoxPrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(glm::vec3 a_v3NewDimensions);
	
	//Update/Draw Functions - have no implementation as primatives
	//cannot be created 
	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

private:
	glm::vec3 m_v3BoxDimensions;
};

#endif //!__BOX_PRIMITIVE_COMPONENT_H__