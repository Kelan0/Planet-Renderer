#include "MeshComponent.h"
#include <string>
#include "../../../renderer/ShaderProgram.h"
#include "../../../VoxelGame.h"
#include "../../../core/Player.h"
#include <chrono>
#include <iostream>

MeshComponent::MeshComponent(Mesh* mesh): _mesh(mesh), _texture(nullptr), _cullMode(GL_BACK), _sfactor(GL_ONE), _dfactor(GL_ONE_MINUS_SRC_ALPHA), _useBlending(true), _useLighting(true)
{
}

MeshComponent::~MeshComponent()
{
	_texture->dispose();
	delete _mesh;
	delete _texture;
}

Mesh* MeshComponent::getMesh() const
{
	return _mesh;
}

Texture* MeshComponent::getTexture() const
{
	return _texture;
}

GLenum MeshComponent::getCullmode() const
{
	return _cullMode;
}

GLenum MeshComponent::getBlendSFactor() const
{
	return _sfactor;
}

GLenum MeshComponent::getBlendDFactor() const
{
	return _dfactor;
}

bool MeshComponent::getUseLighting() const
{
	return _useLighting;
}

bool MeshComponent::getUseBlending() const
{
	return _useBlending;
}

MeshComponent* MeshComponent::setMesh(Mesh* mesh)
{
	_mesh = mesh;

	return this;
}

MeshComponent* MeshComponent::setTexture(Texture* texture)
{
	_texture = texture;

	return this;
}

MeshComponent* MeshComponent::setCullmode(GLenum cullMode)
{
	_cullMode = cullMode;

	return this;
}

MeshComponent* MeshComponent::setUseLighting(bool useLighting)
{
	_useLighting = useLighting;

	return this;
}

MeshComponent* MeshComponent::setBlending(bool useBlending, GLenum sFactor, GLenum dFactor)
{
	_useBlending = useBlending;
	_sfactor = sFactor;
	_dfactor = dFactor;

	return this;
}

MeshComponent* MeshComponent::setPreRenderFunc(void(* function)(double, GameObject*, ShaderProgram*))
{
	this->preRenderFunc = function;
	return this;
}

MeshComponent* MeshComponent::setPostRenderFunc(void(* function)(double, GameObject*, ShaderProgram*))
{
	this->postRenderFunc = function;
	return this;
}

void MeshComponent::init(GameObject* gameObject)
{

}

void MeshComponent::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	Transformation transform = gameObject->getAbsoluteTransformation();
	transform.position -= VoxelGame::player->getCamera()->getPosition();
	glm::mat4x4 trans = glm::mat4x4(transform.getTransformationMatrix()); //TODO: performance impact, fix this
	shaderProgram->setUniform("modelMatrix", trans); //TODO: performance impact, fix this
	shaderProgram->setUniform("useLighting", _useLighting); //TODO: performance impact, fix this

	if (_texture != nullptr)
	{
		shaderProgram->setUniform("useTexture", true);
		shaderProgram->setUniform(const_cast<char*>(("texture" + std::to_string(_texture->getSamplerID())).c_str()), 0);
		_texture->bind();
	}

	if (preRenderFunc != nullptr)
	{
		preRenderFunc(delta, gameObject, shaderProgram);
	}

	_mesh->render();
	shaderProgram->setUniform("useTexture", false);

	if (postRenderFunc != nullptr)
	{
		postRenderFunc(delta, gameObject, shaderProgram);
	}
//	auto b = std::chrono::high_resolution_clock::now();
//	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count() << "nanoseconds to render\n";
}

void MeshComponent::physics(double delta, GameObject* gameObject)
{

}

void MeshComponent::input(double delta, GameObject* gameObject)
{

}
