#pragma once
#include <vector>
#include "ShaderProgram.h"

struct ScreenText
{
	std::string text;
	unsigned int colour;
	int x, y;
	float size;
};

struct FrameBuffer
{
	GLuint FBO;
	GLuint colour;
	GLuint depth;
	GLuint width;
	GLuint height;
};

class RenderingHandler
{
private:
	std::vector<ShaderProgram*> _shaderPrograms;
	ShaderProgram* postProcessing;
	GLenum _polyMode;
	glm::mat4x4 _projectionMatrix;
	FrameBuffer fb;
	float _FOV;
	float _nearPlane;
	float _farPlane;

public:
	RenderingHandler();
	~RenderingHandler();

	void init();

	void preRender();

	void postRender();

	void addShaderProgram(ShaderProgram* shaderProgram);

	void setPolyMode(GLenum polyMode);

	void setFOV(float FOV);

	void setNearPlane(float nearPlane);

	void setFarPlane(float farPlane);

	void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar) const;

	GLenum getPolyMode() const;

	float getFOV() const;

	float getNearPlane() const;

	float getFarPlane() const;

	glm::mat4x4 getProjectionMatrix() const;

	glm::mat4x4 calculateProjectionMatrix(float fov, float near, float far);
};

