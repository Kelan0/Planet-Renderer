#pragma once
#include <vector>
#include "../mesh/Mesh.h"
#include "../../world/sceneGraph/components/MeshComponent.h"

class ShaderProgram;

class GuiScreen
{
private:
	std::vector<MeshComponent*> guis;

	public:
	GuiScreen();
	~GuiScreen();

	void render(ShaderProgram* shaderProgram);

	void renderText(int x, int y, float size, glm::vec4 colour);
};

