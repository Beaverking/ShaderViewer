//
//  Renderer.cpp
//  ShaderViewer
//
//  Created by Max on 21/07/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#include "Renderer.h"
#include "ShaderLoader.h"
#include <fstream>

//ligth pos (5.2f, -9.7, 5.3)


Renderer::Renderer()
: vertices(NULL)
, indices(NULL)
, currentVertexIndex(0)
{
	projectionM = glm::ortho(0.0f, (float)RENDER_WIDTH, (float)RENDER_HEIGHT, 0.0f, -1.0f, 1.0f);
	viewM = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelM = glm::mat4(1);
    MVP = projectionM * viewM * modelM;
}

Renderer::~Renderer()
{
	if (vertices != NULL)
		delete[] vertices;
	if (indices != NULL)
		delete[] indices;
	currentVertexIndex = 0;
	currentIndexIndex = 0;
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

	vertices = new Vertex[kTotalVerticesNumber];
	indices = new GLushort[kTotalIndicesNumber];   //space for indices enough to render all vertices with separate triangles
	currentVertexIndex = 0;
	currentIndexIndex = 0;
	currentTextureIndex = 0;
	width = height = 0;
	currentShadeMode = -1;

	pixelSize[0] = 1.0 / static_cast<float>(RENDER_WIDTH);
	pixelSize[1] = 1.0 / static_cast<float>(RENDER_HEIGHT);

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &vertexBufferId);
	glGenBuffers(1, &indexBufferId);

	textureProgram.programId = ShaderLoader::LoadShaders("Vertex.glsl", "Fragment.glsl", "");
	UseShader(textureProgram);

	glBufferData(GL_ARRAY_BUFFER, kTotalVerticesNumber * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);  //first buffers fill
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kTotalIndicesNumber * sizeof(GLushort), NULL, GL_DYNAMIC_DRAW);

	//Init particle data
	for (int i = 0; i < TOTAL_NUM_PARTICLES; ++i)
	{
		particleBuffer[i].r = RandomNormal();
		particleBuffer[i].g = RandomNormal();
		particleBuffer[i].b = RandomNormal();
		particleBuffer[i].a = 1.0f;
		particleBuffer[i].pX = RENDER_WIDTH * RandomNormal();
		particleBuffer[i].pY = RENDER_HEIGHT * RandomNormal();
		particleBuffer[i].velX = RandomNormal() * 2.0f - 1.0f;
		particleBuffer[i].velY = RandomNormal() * 2.0f - 1.0f;
	}
	particleBuffer[0].pX = particleBuffer[0].pY = 40;
	particleBuffer[1].pX = RENDER_WIDTH - 40;
	particleBuffer[1].pY = RENDER_HEIGHT - 40;

	//glUniform1i(textureProgram.textureSamplerSlot, 0);
	//*************
	//-------------Particle stuff
	//*************
	//Shader storage
	glGenBuffers(1, &shaderStorageBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle)* TOTAL_NUM_PARTICLES, &particleBuffer[0], GL_DYNAMIC_COPY);

	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //unbind

	particleProgram.programId = ShaderLoader::LoadShaders("ParticleVert.glsl", "ParticleFrag.glsl", "ParticleGeom.glsl");
	UseShader(particleProgram);

	//Compute shader
	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	printf("Compute program: max global work group size x:%i y:%i z:%i\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	particleComputeProgram.programId = ShaderLoader::LoadComputeShader("particleCompute.glsl");
	UseShader(particleComputeProgram);

	//enabling texturing
	//glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
    
	return 0;
}

void Renderer::UseShader(const ProgramDataBase& program)
{
	if (currentProgram.programId == program.programId)
		return;

	if (currentVertexIndex > 0)
		DrawCurrentData();
	currentProgram = program;
	glUseProgram(currentProgram.programId);
	if (currentProgram.programId == textureProgram.programId)
	{
		textureProgram.matrixSlot = glGetUniformLocation(textureProgram.programId, "MVP");
		glUniformMatrix4fv(textureProgram.matrixSlot, 1, GL_FALSE, &MVP[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);	//using vertex data from vbo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

		textureProgram.positionSlot = glGetAttribLocation(textureProgram.programId, "position");    //Binding vertex attribute slots to shader's input variables
		textureProgram.colorSlot = glGetAttribLocation(textureProgram.programId, "vertexColor");
		textureProgram.textureCoordSlot = glGetAttribLocation(textureProgram.programId, "textureCoord");
		textureProgram.textureSamplerSlot = glGetUniformLocation(textureProgram.programId, "texUnit");

		glEnableVertexAttribArray(textureProgram.positionSlot);
		glEnableVertexAttribArray(textureProgram.colorSlot);
		glEnableVertexAttribArray(textureProgram.textureCoordSlot);

		glVertexAttribPointer(textureProgram.positionSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(textureProgram.colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 2));
		glVertexAttribPointer(textureProgram.textureCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 6));

		glUniform1i(textureProgram.textureSamplerSlot, 0);
	}
	else if (currentProgram.programId == particleProgram.programId)
	{
		particleProgram.matrixSlot = glGetUniformLocation(textureProgram.programId, "MVP");
		glUniformMatrix4fv(particleProgram.matrixSlot, 1, GL_FALSE, &MVP[0][0]);

		//using vertex data from ssbo
		glBindBuffer(GL_ARRAY_BUFFER, shaderStorageBufferId);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 2));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 4));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		pixelSize[0] = 1.0 / static_cast<float>(RENDER_WIDTH)*4;
		pixelSize[1] = 1.0 / static_cast<float>(RENDER_HEIGHT)*4;

		particleProgram.pixelSizeSlot = glGetUniformLocation(particleProgram.programId, "pixelSize");
		glUniform2f(particleProgram.pixelSizeSlot, pixelSize[0], pixelSize[1]);
		//particleProgram.blockIndex = glGetProgramResourceIndex(particleProgram.programId, GL_SHADER_STORAGE_BLOCK, "particle_data");
	}
	else if (currentProgram.programId == particleComputeProgram.programId)
	{
		//using data from ssbo itself
		//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT); MEMORY BARRIER!??????
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderStorageBufferId);

		//particleComputeProgram.blockIndex = glGetProgramResourceIndex(particleComputeProgram.programId, GL_SHADER_STORAGE_BLOCK, "particleBuffer");
		//glShaderStorageBlockBinding(particleProgram.programId, particleComputeProgram.blockIndex, 11);	//11 - hardcoded in glsl

		particleComputeProgram.gravitySlot = glGetUniformLocation(particleComputeProgram.programId, "gravity");
		particleComputeProgram.dtSlot = glGetUniformLocation(particleComputeProgram.programId, "dt");
		particleComputeProgram.renderSizeSlot = glGetUniformLocation(particleComputeProgram.programId, "renderSize");

		glUniform1f(particleComputeProgram.gravitySlot, SIM_GRAVITY);
		glUniform1f(particleComputeProgram.dtSlot, static_cast<GLfloat>(MS_PER_FRAME));
		glUniform2f(particleComputeProgram.renderSizeSlot, static_cast<GLfloat>(RENDER_WIDTH), static_cast<GLfloat>(RENDER_HEIGHT));
	}

	GLuint err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
}

void Renderer::SimulateParticles()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, shaderStorageBufferId);
	UseShader(particleComputeProgram);
	glDispatchCompute(90, 1, 1);	//HARDCODED!!!								!!!!				!!!!!
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void Renderer::OnFinishDraw()
{

}

void Renderer::Deinit()
{
	if (currentProgram.programId == textureProgram.programId)
	{
		glDisableVertexAttribArray(textureProgram.positionSlot);
		glDisableVertexAttribArray(textureProgram.colorSlot);
		glDisableVertexAttribArray(textureProgram.textureCoordSlot);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	//glDisableVertexAttribArray(normalVectorSlot);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &indexBufferId);
	glDeleteBuffers(1, &shaderStorageBufferId);
	glDeleteProgram(textureProgram.programId);
	glDeleteProgram(particleProgram.programId);
	//glDeleteProgram(shadowVolumeProgram);	//deleting shader program

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

const Vec2f Renderer::GetFrameSize()
{
	return Vec2f(RENDER_WIDTH, RENDER_HEIGHT);
}

void Renderer::DrawCurrentData()
{
	if (currentIndexIndex == 0)
		return;

	glBufferData(GL_ARRAY_BUFFER, kTotalVerticesNumber * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kTotalIndicesNumber * sizeof(GLushort), indices, GL_DYNAMIC_DRAW);

	glDrawElements(GL_TRIANGLE_STRIP, currentIndexIndex, GL_UNSIGNED_SHORT, 0);

	currentVertexIndex = 0;
	currentIndexIndex = 0;
}

void Renderer::DrawParticles()
{
	UseShader(particleProgram);
	glDrawArrays(GL_POINTS, 0, TOTAL_NUM_PARTICLES);
}

void Renderer::SetTexture(int tIndex)
{
	if (currentTextureIndex != tIndex)
	{
		if (currentTextureIndex != 0)
			DrawCurrentData();
		glBindTexture(GL_TEXTURE_2D, tIndex);
	}
	currentTextureIndex = tIndex;
}

void Renderer::SetShadeMode(Renderer::ShadeMode mode)
{
	if (currentShadeMode != mode)
	{
		if (currentShadeMode != -1)
			DrawCurrentData();
	}
	currentShadeMode = mode;
}

void Renderer::GetQuadVertices(Vec2f *verts, const ImageP img, float angle, float scaleX, float scaleY)
{
	float iWidth = static_cast<float>(img->w);
	float iHeight = static_cast<float>(img->h);
	float rotationPivotX = img->pivotX == -1 ? img->w * 0.5f : img->pivotX;
	float rotationPivotY = img->pivotY == -1 ? img->h * 0.5f : img->pivotY;
	float localX0 = -rotationPivotX;
	float localY0 = -rotationPivotY;
	float localX1 = localX0 + iWidth;
	float localY1 = localY0 + iHeight;
	float sina = angle == 0.0f ? 0.0f : sinf(angle * PI / 180.0f);
	float cosa = angle == 0.0f ? 1.0f : cosf(angle * PI / 180.0f);

	localX0 *= scaleX;
	localX1 *= scaleX;
	localY0 *= scaleY;
	localY1 *= scaleY;

	verts[0].x = localX0;
	verts[0].y = localY0;
	verts[1].x = localX1;
	verts[1].y = localY0;
	verts[2].x = localX0;
	verts[2].y = localY1;
	verts[3].x = localX1;
	verts[3].y = localY1;
	for (int i = 0; i < 4; ++i)
		verts[i] = Vec2f(cosa * verts[i].x - sina * verts[i].y, sina * verts[i].x + cosa * verts[i].y);
}

void Renderer::DrawSolidQuad(float x, float y, float x1, float y1, const Color4f &col)
{
	/*UseShader(shaderSolidColor);
	if (currentVertexIndex + 4 >= kTotalVerticesNumber || currentIndexIndex + 6 >= kTotalIndicesNumber)
		DrawCurrentData();

	Vec2f uvs[4] = { Vec2f(), Vec2f(), Vec2f(), Vec2f() };
	Vec2f positions[4] = { Vec2f(x, y), Vec2f(x1, y), Vec2f(x, y1), Vec2f(x1, y1) };
	SetVertexQuadData(positions, uvs, col);*/
}

void Renderer::DrawSolidQuad(float x, float y, int width, int height, const Color4f &col, float angle)
{
	/*UseShader(shaderSolidColor);
	if (currentVertexIndex + 4 >= kTotalVerticesNumber || currentIndexIndex + 6 >= kTotalIndicesNumber)
		DrawCurrentData();

	Vec2f quadVerts[4];
	quadVerts[0].x = x;
	quadVerts[0].y = y;
	quadVerts[1].x = x + width;
	quadVerts[1].y = y;
	quadVerts[2].x = x;
	quadVerts[2].y = y + height;
	quadVerts[3].x = x + width;
	quadVerts[3].y = y + height;
	if (angle != 0.0f)
	{
		float pivotX = x + width / 2.0f;
		float pivotY = y + height / 2.0f;
		for (int i = 0; i < 4; i++)
		{
			float px = quadVerts[i].x - pivotX;
			float py = quadVerts[i].y - pivotY;
			float xnew = px * cosf(angle) - py * sinf(angle);
			float ynew = px * sinf(angle) + py * cosf(angle);
			quadVerts[i].x = xnew + pivotX;
			quadVerts[i].y = ynew + pivotY;
		}
	}
	Vec2f uvs[4] = { Vec2f(), Vec2f(), Vec2f(), Vec2f() };
	SetVertexQuadData(quadVerts, uvs, col);*/
}

void Renderer::DrawSprite(ImageP img, float x, float y, float a, bool centered, float angle, float scaleVert, float scaleHoriz)
{
	UseShader(textureProgram);
	if (currentVertexIndex + 4 >= kTotalVerticesNumber || currentIndexIndex + 6 >= kTotalIndicesNumber)
		DrawCurrentData();

	SetTexture(img->tID);
	float drawX = x, drawY = y;
	if (centered)
	{
		drawX -= img->w * 0.5f;
		drawY -= img->h * 0.5f;
	}
	drawX = floorf(drawX);
	drawY = floorf(drawY);

	Vec2f polyVerts[4];
	Color4f premulColor = Color4f(1.0f, 1.0f, 1.0f, a);
	GetQuadVertices(polyVerts, img, angle, scaleVert, scaleHoriz);
	for (int i = 0; i < 4; i++)
	{
		polyVerts[i].x += drawX + img->w * 0.5f;
		polyVerts[i].y += drawY + img->h * 0.5f;
	}
	Vec2f uvs[4] = { Vec2f(img->u, img->v), Vec2f(img->u1, img->v), Vec2f(img->u, img->v1), Vec2f(img->u1, img->v1) };
	SetVertexQuadData(polyVerts, uvs, premulColor);
}

void Renderer::SetVertexQuadData(Vec2f *positions, Vec2f *uvs, const Color4f &color)
{
	if (currentIndexIndex > 0)
	{
		indices[currentIndexIndex + 1] = indices[currentIndexIndex];
		currentIndexIndex += 2;
	}

	memcpy(&vertices[currentVertexIndex].r, &color, sizeof(Color4f));
	vertices[currentVertexIndex].x = positions[0].x;
	vertices[currentVertexIndex].y = positions[0].y;
	vertices[currentVertexIndex].u = uvs[0].x;
	vertices[currentVertexIndex].v = uvs[0].y;
	indices[currentIndexIndex++] = currentVertexIndex++;

	memcpy(&vertices[currentVertexIndex].r, &color, sizeof(Color4f));
	vertices[currentVertexIndex].x = positions[1].x;
	vertices[currentVertexIndex].y = positions[1].y;
	vertices[currentVertexIndex].u = uvs[1].x;
	vertices[currentVertexIndex].v = uvs[1].y;
	indices[currentIndexIndex++] = currentVertexIndex++;

	memcpy(&vertices[currentVertexIndex].r, &color, sizeof(Color4f));
	vertices[currentVertexIndex].x = positions[2].x;
	vertices[currentVertexIndex].y = positions[2].y;
	vertices[currentVertexIndex].u = uvs[2].x;
	vertices[currentVertexIndex].v = uvs[2].y;
	indices[currentIndexIndex++] = currentVertexIndex++;

	memcpy(&vertices[currentVertexIndex].r, &color, sizeof(Color4f));
	vertices[currentVertexIndex].x = positions[3].x;
	vertices[currentVertexIndex].y = positions[3].y;
	vertices[currentVertexIndex].u = uvs[3].x;
	vertices[currentVertexIndex].v = uvs[3].y;
	indices[currentIndexIndex++] = currentVertexIndex++;
	indices[currentIndexIndex] = indices[currentIndexIndex - 1];
	currentIndexIndex++;
}

//-------------------------------------------------------------------------

void Renderer::OnDraw()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	DrawCurrentData();	//drawing vertices

	//drawing particles
	DrawParticles();

	OnFinishDraw();

	// Swap buffers
	glfwSwapBuffers(window);


	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
}