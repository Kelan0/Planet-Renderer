#pragma once
#include <vector>
#include "../../renderer/mesh/Mesh.h"
#include "../../renderer/ShaderProgram.h"
#include "../../util/WorldConstants.h"

class GameComponent;
class GameObject;
class GameBase;

class GameBase
{
public:
	virtual ~GameBase() {}

	virtual void init(GameObject* gameObject) = 0;

	virtual void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) = 0;

	virtual void physics(double delta, GameObject* gameObject) = 0;

	virtual void input(double delta, GameObject* gameObject) = 0;
};

class GameComponent : public GameBase
{
public:
	virtual ~GameComponent() {}

	virtual void init(GameObject* gameObject) = 0;

	virtual void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) = 0;

	virtual void physics(double delta, GameObject* gameObject) = 0;

	virtual void input(double delta, GameObject* gameObject) = 0;
};

class GameObject : public GameBase
{
protected:
	std::vector<GameObject*> _children;
	std::vector<GameComponent*> _components;

	GameObject* _parent;
	Transformation* _transformation;

	bool _inheritPosition;
	bool _inheritRotation;
	bool _inheritScale;

public:
	GameObject(Transformation* transformation);
	GameObject() : GameObject(new Transformation()) {}

	~GameObject();

	void init(GameObject* gameObject) override;

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	void physics(double delta, GameObject* gameObject) override;

	void input(double delta, GameObject* gameObject) override;

	void addChild(GameObject* child);

	void addComponent(GameComponent* component);

	void setParent(GameObject* parent);

	void setTransformation(Transformation transformation);

	void setInheritPosition(bool inheritPosition);

	void setInheritRotation(bool inheritRotation);

	void setInheritScale(bool inheritScale);

	bool doInheritPosition() const;
	
	bool doInheritRotation() const;
	
	bool doInheritScale() const;

	GameObject* getParent() const;

	Transformation* getRelativeTransformation();

	Transformation getAbsoluteTransformation(bool doScale = true);
};
