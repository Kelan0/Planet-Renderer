#pragma once
#include "MeshComponent.h"

class RenderComponent : public GameComponent
{
private:
	void(*_renderFunc)(double delta, GameObject* gameObject, ShaderProgram* shaderProgram);

public:
	RenderComponent(void(*renderFunc)(double delta, GameObject* gameObject, ShaderProgram* shaderProgram));
	~RenderComponent();

	void init(GameObject* gameObject) override;

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	void physics(double delta, GameObject* gameObject) override;

	void input(double delta, GameObject* gameObject) override;

	void setRenderFunc(void(*renderFunc)(double delta, GameObject* gameObject, ShaderProgram* shaderProgram));
};

