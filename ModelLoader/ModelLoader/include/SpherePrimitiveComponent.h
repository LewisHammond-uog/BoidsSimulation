#ifndef __BOX_PRIMITIVE_COMPONENT_H__
#define __BOX_PRIMITIVE_COMPONENT_H__

//Project Includes
#include <glm/glm.hpp>

#include "PrimitiveComponent.h"

class SpherePrimitiveComponent : public PrimitiveComponent
{
public:
	SpherePrimitiveComponent(Entity* a_pOwner, float a_fSphereRadius = 1.0f);
	~SpherePrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(float a_fNewRadius);
	
	//Update/Draw Functions - have no implementation as primatives
	//cannot be created 
	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

private:
	float m_fSphereRadius;
	//Number of rows/columns to draw the circle our of
	//a higher number means a more precise sphere
	int m_iSphereResolution = 10;
};


#endif //!__BOX_PRIMITIVE_COMPONENT_H__
