//
//  Renderer.cpp
//  ShaderViewer
//
//  Created by Max on 21/07/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#include "Renderer.h"
#include "ShaderLoader.h"
#include "Lodepng.h"
#include <fstream>

//ligth pos (5.2f, -9.7, 5.3)


Renderer::Renderer()
{
	projectionM = glm::ortho(0.0f, (float)RENDER_WIDTH, (float)RENDER_HEIGHT, 0.0f, -1.0f, 1.0f);
	viewM = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelM = glm::mat4(1.0f);
    MVP = projectionM * viewM * modelM;

	backgroundWidth = backgroundHeight = 0;
}

Renderer::~Renderer()
{
    
}

void OrthoProjM(float left, float right, float bottom, float top, float near, float far, float *m)
{
	float w = right - left;
	float h = top - bottom;
	float dist = far - near;
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	m[0] = 2.0f / w;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 2.0f / h;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = -2.0f / dist;
	m[11] = 0.0f;

	m[12] = tx;
	m[13] = ty;
	m[14] = tz;
	m[15] = 1.0f;
}

int Renderer::Initialize()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(RENDER_WIDTH, RENDER_HEIGHT, "Shader viewer", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	GLint vMagor, vMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &vMagor);
	glGetIntegerv(GL_MINOR_VERSION, &vMinor);
	printf("OpenGL version %d.%d\n", vMagor, vMinor);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
	colorProgarm = ShaderLoader::LoadShaders("Vertex.glsl", "Fragment.glsl", "");

	printf("Loading image..\n");
	backgroundWidth = BACKGROUND_WIDTH;
	backgroundHeight = BACKGROUND_HEIGHT;
	uint result = lodepng::decode(backgroundImageData, backgroundWidth, backgroundHeight, BACKGROUND_FILE_NAME);
	if (result != 0)
	{
		printf("Decode texture failed\n");
		return -1;
	}

	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//texture
	glGenTextures(1, &backGroundId);
	glBindTexture(GL_TEXTURE_2D, backGroundId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, backgroundWidth, backgroundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &backgroundImageData[0]);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	
	glUseProgram(colorProgarm);

	positionSlot = glGetAttribLocation(colorProgarm, "position");    //Binding vertex attribute slots to shader's input variables
	textureCoordSlot = glGetAttribLocation(colorProgarm, "textureCoord");
	//textureSamplerSlot = glGetUniformLocation(depthProgram, "DepthMap");
	//normalVectorSlot = glGetAttribLocation(currentProgram, "NormalIn");
	matrixSlot = glGetUniformLocation(colorProgarm, "MVP");
	//sceneFlagSlot = glGetUniformLocation(depthProgram, "DrawScene");
	//viewMatrixSlot = glGetUniformLocation(currentProgram, "V");
	//modelMatrixSlot = glGetUniformLocation(currentProgram, "M");
	//lightPositionSlot = glGetUniformLocation(currentProgram, "LightPosition_worldspace");
	textureSamplerSlot = glGetUniformLocation(colorProgarm, "texUnit");

	OrthoProjM(0.0f, static_cast<float>(RENDER_WIDTH), static_cast<float>(RENDER_HEIGHT), 0.0f, -1.0f, 1.0f, projectionMatrix);
	glUniformMatrix4fv(matrixSlot, 1, GL_FALSE, projectionMatrix);
	//glUniformMatrix4fv(modelMatrixSlot, 1, GL_FALSE, &modelM[0][0]);
	//glUniformMatrix4fv(viewMatrixSlot, 1, GL_FALSE, &viewM[0][0]);
	//glUniform3f(lightPositionSlot, lightPos.x, lightPos.y, lightPos.z);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}


	glEnableVertexAttribArray(positionSlot);
	glEnableVertexAttribArray(textureCoordSlot);
	//glEnableVertexAttribArray(normalVectorSlot);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(textureCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 2));
	//glVertexAttribPointer(normalVectorSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 5));


	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);

	quadVerts[0] = Vertex(0.0f, 0.0f, 0.0f, 0.0f);
	quadVerts[1] = Vertex((float)RENDER_WIDTH, 0.0f, 1.0f, 0.0f);
	quadVerts[2] = Vertex(0.0f, (float)RENDER_HEIGHT, 0.0f, 1.0f);
	quadVerts[3] = Vertex((float)(RENDER_WIDTH), (float)(RENDER_HEIGHT), 1.0f, 1.0f);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* 4, quadVerts, GL_STATIC_DRAW);
	glUniform1i(textureSamplerSlot, 0);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
    
	return 0;
}

void Renderer::Deinit()
{
	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(textureCoordSlot);
	//glDisableVertexAttribArray(normalVectorSlot);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteProgram(colorProgarm);
	//glDeleteProgram(shadowVolumeProgram);	//deleting shader program


	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void Renderer::OnDraw()
{
	// Use our shader
	glUseProgram(colorProgarm);

	//glUniform1f(sceneFlagSlot, 1.0f);
	//feeding buffer with scene data
	//Draw scene into depth buffer
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glDrawArrays(mesh.LoadedWithAdjacency() ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES, 0, mesh.GetNumIndices());	//drawing scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glUniform1f(sceneFlagSlot, 0.0f);
	//feeding buffer with quad data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* 4, quadVerts, GL_STATIC_DRAW);
	//Drawing to color buffer
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	//feeding the shader with depth texture
	//SetReadFromDepth();
	//drawing quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	

	// Draw the triangle !
	//glDrawArrays(GL_TRIANGLES, 0, currentVertexIndex); // 3 indices starting at 0 -> 1 triangle

	// Swap buffers
	glfwSwapBuffers(window);


	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
}