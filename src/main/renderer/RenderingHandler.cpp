//#define FT2_BUILD_LIBRARY

#include "RenderingHandler.h"
#include <glm/gtc/constants.inl>
#include <glm/gtc/matrix_transform.inl>
#include "WindowHandler.h"
#include "../VoxelGame.h"
#include <iostream>
#include "mesh/Mesh.h"
#include "../core/Player.h"
#include "../Engine.h"


RenderingHandler::RenderingHandler(): _polyMode(GL_FILL), _FOV(70.0F), _nearPlane(0.1), _farPlane(2000.0F)
{
}


RenderingHandler::~RenderingHandler()
{
}

void RenderingHandler::init()
{
	glGenFramebuffers(1, &fb.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.FBO);
	glGenTextures(1, &fb.colour);
	glBindTexture(GL_TEXTURE_2D, fb.colour);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowHandler::getWidth(), WindowHandler::getHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &fb.depth);
	glBindRenderbuffer(GL_RENDERBUFFER, fb.depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowHandler::getWidth(), WindowHandler::getHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb.depth);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.colour, 0);

	postProcessing = new ShaderProgram();
	postProcessing->addShader(GL_VERTEX_SHADER, "res/shaders/postProcessing/VertexShader.glsl");
	postProcessing->addShader(GL_FRAGMENT_SHADER, "res/shaders/postProcessing/FragmentShader.glsl");
	postProcessing->addAttribute(ATTRIBUTE_VERTEX_GEOMETRIC, "position");
	postProcessing->addAttribute(ATTRIBUTE_VERTEX_NORMAL, "normal");
	postProcessing->addAttribute(ATTRIBUTE_VERTEX_TEXTURE, "texture");
	postProcessing->addAttribute(ATTRIBUTE_VERTEX_COLOUR, "colour");
	postProcessing->completeProgram();
}

void RenderingHandler::preRender()
{
	_projectionMatrix = calculateProjectionMatrix(_FOV, _nearPlane, _farPlane);
	glm::mat4x4 viewMatrix = glm::mat4x4(VoxelGame::player->getCamera()->getViewMatrix());

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fb.FBO);

	glPolygonMode(GL_FRONT_AND_BACK, _polyMode);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < _shaderPrograms.size(); i++)
	{
		ShaderProgram* shaderProgram = _shaderPrograms[i];

		if (shaderProgram != nullptr)
		{
			float Fcoef = 2.0F / log2(_farPlane + 1.0F);
			shaderProgram->useProgram(true);
			shaderProgram->setUniform("Fcoef", Fcoef);
			shaderProgram->setUniform("near", _nearPlane);
			shaderProgram->setUniform("far", _farPlane);
			shaderProgram->setUniform("projectionMatrix", _projectionMatrix);
			shaderProgram->setUniform("viewMatrix", viewMatrix);
			shaderProgram->setUniform("scale", VoxelGame::scale);
			shaderProgram->useProgram(false);
		}
	}
}

void RenderingHandler::postRender()
{
	postProcessing->useProgram(true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, fb.depth);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glBegin(GL_TRIANGLES);
	glVertexAttrib3f(0, -1.0, -1.0, 0.1); //V0
	glVertexAttrib3f(0, +1.0, -1.0, 0.1); //V3
	glVertexAttrib3f(0, -1.0, +1.0, 0.1); //V1

	glVertexAttrib3f(0, -1.0, +1.0, 0.1); //V1
	glVertexAttrib3f(0, +1.0, -1.0, 0.1); //V3
	glVertexAttrib3f(0, +1.0, +1.0, 0.1); //V2

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnd();

	postProcessing->useProgram(false);
}

void RenderingHandler::addShaderProgram(ShaderProgram* shaderProgram)
{
	if (shaderProgram != nullptr)
	{
		auto it = std::find(_shaderPrograms.begin(), _shaderPrograms.end(), shaderProgram);

		if (it == _shaderPrograms.end())
		{
			_shaderPrograms.push_back(shaderProgram);
		}
	}
}

void RenderingHandler::setPolyMode(GLenum polyMode)
{
	_polyMode = polyMode;
}

void RenderingHandler::setFOV(float FOV)
{
	_FOV = FOV;
}

void RenderingHandler::setNearPlane(float nearPlane)
{
	_nearPlane = nearPlane;
}

void RenderingHandler::setFarPlane(float farPlane)
{
	_farPlane = farPlane;
}

void RenderingHandler::perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar) const
{
	GLdouble fW, fH;

	fH = tan(fovY / 360 * glm::pi<float>()) * zNear;
	fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

GLenum RenderingHandler::getPolyMode() const
{
	return _polyMode;
}

glm::mat4x4 RenderingHandler::getProjectionMatrix() const
{
	return _projectionMatrix;
}

glm::mat4x4 RenderingHandler::calculateProjectionMatrix(float fov, float near, float far)
{
	return glm::perspective(fov, WindowHandler::getAspectRatio(), near, far);
}

float RenderingHandler::getFOV() const
{
	return _FOV;
}

float RenderingHandler::getNearPlane() const
{
	return _nearPlane;
}

float RenderingHandler::getFarPlane() const
{
	return _farPlane;
}
