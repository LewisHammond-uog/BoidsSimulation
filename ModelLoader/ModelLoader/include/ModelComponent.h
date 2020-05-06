#ifndef __MODEL_COMPONENT_H__
#define __MODEL_COMPONENT_H__

//Project Includes
#include "Component.h"

//Forward Declare
class Model;

class ModelComponent : public Component
{
public:
	ModelComponent(Entity* a_pOwner);
	~ModelComponent() = default;

	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override;

	void SetModel(Model* a_pNewModel) { m_pModelData = a_pNewModel; };
	void SetScale(const float a_fNewScale) { m_fModelScale = a_fNewScale; };

private:
	Model* m_pModelData;
	float m_fModelScale;

};

#endif // ! __MODEL_COMPONENT_H__

