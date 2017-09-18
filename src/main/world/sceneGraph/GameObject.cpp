#include "SceneGraph.h"
#include "../../VoxelGame.h"

GameObject::GameObject(Transformation* transformation): _transformation(transformation), _inheritPosition(true), _inheritRotation(false), _inheritScale(true), _parent(nullptr)
{
}


GameObject::~GameObject()
{
}

void GameObject::init(GameObject* gameObject)
{
	for (int i = 0; i < _components.size(); i++)
	{
		GameComponent* component = _components[i];
		if (component != nullptr)
		{
			component->init(this);
		}
	}

	for (int i = 0; i < _children.size(); i++)
	{
		GameObject* child = _children[i];
		if (child != nullptr)
		{
			child->init(this);
		}
	}
}

void GameObject::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	for (int i = 0; i < _components.size(); i++)
	{
		GameComponent* component = _components[i];
		if (component != nullptr)
		{
			component->render(delta, this, shaderProgram);
		}
	}

	for (int i = 0; i < _children.size(); i++)
	{
		GameObject* child = _children[i];
		if (child != nullptr)
		{
			child->render(delta, this, shaderProgram);
		}
	}
}

void GameObject::physics(double delta, GameObject* gameObject)
{
	for (int i = 0; i < _components.size(); i++)
	{
		GameComponent* component = _components[i];
		if (component != nullptr)
		{
			component->physics(delta, this);
		}
	}

	for (int i = 0; i < _children.size(); i++)
	{
		GameObject* child = _children[i];
		if (child != nullptr)
		{
			child->physics(delta, this);
		}
	}
}

void GameObject::input(double delta, GameObject* gameObject)
{
	for (int i = 0; i < _components.size(); i++)
	{
		GameComponent* component = _components[i];
		if (component != nullptr)
		{
			component->input(delta, this);
		}
	}

	for (int i = 0; i < _children.size(); i++)
	{
		GameObject* child = _children[i];
		if (child != nullptr)
		{
			child->input(delta, this);
		}
	}
}

void GameObject::addChild(GameObject* child)
{
	if (child != nullptr)
	{
		if (child->_parent != nullptr)
		{
			auto it = std::find(child->getParent()->_children.begin(), child->getParent()->_children.end(), child);

			if (it != child->getParent()->_children.end())
			{
				std::swap(*it, child->getParent()->_children.back());
				child->getParent()->_children.pop_back();
			}
		}
		child->_parent = this;
		this->_children.push_back(child);
	}
}

void GameObject::addComponent(GameComponent* component)
{
	if (component != nullptr)
	{
		_components.push_back(component);
	}
}

void GameObject::setParent(GameObject* parent)
{
	if (parent != nullptr && _parent != parent)
	{
		parent->addChild(this);
	}
}

void GameObject::setTransformation(Transformation transformation)
{
	if (_transformation == nullptr)
	{
		_transformation = new Transformation();
	}

	_transformation->position = transformation.position;
	_transformation->rotation = transformation.rotation;
	_transformation->scale = transformation.scale;
}

void GameObject::setInheritPosition(bool inheritPosition)
{
	_inheritPosition = inheritPosition;
}

void GameObject::setInheritRotation(bool inheritRotation)
{
	_inheritRotation = inheritRotation;
}

void GameObject::setInheritScale(bool inheritScale)
{
	_inheritScale = inheritScale;
}

bool GameObject::doInheritPosition() const
{
	return _inheritPosition;
}

bool GameObject::doInheritRotation() const
{
	return _inheritRotation;
}

bool GameObject::doInheritScale() const
{
	return _inheritScale;
}

GameObject* GameObject::getParent() const
{
	return _parent;
}

Transformation* GameObject::getRelativeTransformation()
{
	return _transformation;
}

Transformation GameObject::getAbsoluteTransformation(bool doScale)
{
	Transformation tb = *this->getRelativeTransformation();

	if (doScale)
	{
		tb.position *= VoxelGame::scale;
		tb.scale *= VoxelGame::scale;
	}

	if (_parent != nullptr)
	{
		Transformation ta = _parent->getAbsoluteTransformation(doScale);

		if (!_inheritPosition) ta.position = glm::dvec3();
		if (!_inheritScale) ta.scale = glm::dvec3();
		if (!_inheritRotation)
		{
			ta.rotation = glm::dvec3();
		}
		else
		{
			tb.position = glm::dvec3(Transformation(glm::dvec3(), ta.rotation, glm::dvec3(1.0)).getTransformationMatrix() * glm::dvec4(tb.position, 1.0));
		}

		return ta + tb;// Transformation(tb.position * VoxelGame::scale, tb.rotation, tb.scale * VoxelGame::scale);
	}

	return tb;
}
