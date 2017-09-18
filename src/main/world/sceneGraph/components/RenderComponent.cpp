#include "RenderComponent.h"

RenderComponent::RenderComponent(void(*renderFunc)(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)) : _renderFunc(renderFunc)
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::init(GameObject* gameObject)
{

}

void RenderComponent::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	_renderFunc(delta, gameObject, shaderProgram);
}

void RenderComponent::physics(double delta, GameObject* gameObject)
{

}

void RenderComponent::input(double delta, GameObject* gameObject)
{

}

void RenderComponent::setRenderFunc(void(* renderFunc)(double delta, GameObject* gameObject, ShaderProgram* shaderProgram))
{
	_renderFunc = renderFunc;
}
