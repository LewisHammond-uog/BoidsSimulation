#ifndef __SPHERE_PRIMITIVE_COMPONENT_H__
#define __SPHERE_PRIMITIVE_COMPONENT_H__

#include "PrimitiveComponent.h"

/// <summary>
/// Component to draw a sphere in the world
/// </summary>
class SpherePrimitiveComponent : public PrimitiveComponent
{
public:
	explicit SpherePrimitiveComponent(Entity* a_pOwner, float a_fSphereRadius = 1.0f);
	~SpherePrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(float a_fNewRadius);
	
	//Update/Draw Functions - have no implementation as primatives
	//cannot be created 
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override;

	//Get text name of the component
	const char* GetComponentName() const override;

private:
	//Radius of the sphere
	float m_fSphereRadius;

	//Number of rows/columns to draw the circle our of a higher number means a more precise sphere
	int m_iSphereResolution = 10;

	const char* m_szName = "Primitive (Box)";
};


#endif //!__SPHERE_PRIMITIVE_COMPONENT_H__
