#ifndef __MODEL_COMPONENT_H__
#define __MODEL_COMPONENT_H__

//Project Includes
#include <vector>

#include "Component.h"

//Forward Declare
class Model;

/// <summary>
/// Component to deal with the rendering of a model
/// </summary>
class ModelComponent : public Component
{
public:
	explicit ModelComponent(Entity* a_pOwner);
	~ModelComponent() = default;

	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override;

	void SetModel(Model* a_pNewModel) { m_pModelData = a_pNewModel; };
	void ChooseRandomModel(std::vector<Model*>& a_vpModels);
	void SetScale(const float a_fNewScale) { m_fModelScale = a_fNewScale; };


	//Get text name of the component
	const char* GetComponentName() const override;
private:
	Model* m_pModelData;
	float m_fModelScale;

	const char* m_szName = "Model";

};

#endif // ! __MODEL_COMPONENT_H__

