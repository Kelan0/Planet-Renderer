#pragma once
#include "../../../renderer/mesh/Mesh.h"
#include "../../../renderer/mesh/Texture.h"
#include "../SceneGraph.h"

class ShaderProgram;

class MeshComponent : public GameComponent
{
protected:
	Mesh* _mesh;
	Texture* _texture;

	GLenum _cullMode;
	GLenum _sfactor;
	GLenum _dfactor;

	bool _useBlending;
	bool _useLighting;
	void(*preRenderFunc)(double, GameObject*, ShaderProgram*);
	void(*postRenderFunc)(double, GameObject*, ShaderProgram*);

public:
	MeshComponent(Mesh* mesh);
	
	~MeshComponent();

	Mesh* getMesh() const;

	Texture* getTexture() const;

	GLenum getCullmode() const;

	GLenum getBlendSFactor() const;

	GLenum getBlendDFactor() const;

	bool getUseLighting() const;

	bool getUseBlending() const;

	MeshComponent* setMesh(Mesh* mesh);

	MeshComponent* setTexture(Texture* texture);

	MeshComponent* setCullmode(GLenum cullMode);

	MeshComponent* setUseLighting(bool useLighting);

	MeshComponent* setBlending(bool useBlending, GLenum sFactor = GL_ONE, GLenum dFactor = GL_ONE_MINUS_SRC_ALPHA);

	MeshComponent* setPreRenderFunc(void(*function)(double, GameObject*, ShaderProgram*));

	MeshComponent* setPostRenderFunc(void(*function)(double, GameObject*, ShaderProgram*));

	void init(GameObject* gameObject) override;

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	void physics(double delta, GameObject* gameObject) override;

	void input(double delta, GameObject* gameObject) override;
};

