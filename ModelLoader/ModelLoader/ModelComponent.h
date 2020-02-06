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
	~ModelComponent();

	virtual void Update(float a_fDeltaTime) {};
	virtual void Draw(Shader* a_pShader);

	void SetModel(Model* a_pNewModel) { m_pModelData = a_pNewModel; };

private:
	Model* m_pModelData;
	float m_fModelScale;

};

#endif // ! __MODEL_COMPONENT_H__

