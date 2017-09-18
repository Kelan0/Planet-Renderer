#include "GuiScreen.h"
#include "../WindowHandler.h"


GuiScreen::GuiScreen()
{
}


GuiScreen::~GuiScreen()
{
}

void GuiScreen::render(ShaderProgram* shaderProgram)
{
//	glDisable(GL_DEPTH_TEST);

//	for (auto it = guis.begin(); it != guis.end(); ++it)
//	{
//		(*it)->render(shaderProgram);
//	}
}

void GuiScreen::renderText(int x, int y, float size, glm::vec4 colour)
{
//	cimg_library::CImg<unsigned char> imgtext;
//	unsigned char color = 1;
//	imgtext.draw_text(0, 0, "Hello guys !", &color, 0, 1, 23);
//
//	MeshBuilder* mb = MeshBuilder::createPlane(imgtext.width() / WindowHandler::getWidth(), imgtext.height() / WindowHandler::getHeight());
//	MeshComponent* gui = new MeshComponent(new Mesh(mb));
//	//gui->setTransformation(glm::vec3(x, y, 0.0F));
////	gui->setTexture(Texture::loadTexture(imgtext));
//	guis.push_back(gui);
}
