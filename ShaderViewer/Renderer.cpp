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
, currentProgram(-1)
{
	projectionM = glm::ortho(0.0f, (float)RENDER_WIDTH, (float)RENDER_HEIGHT, 0.0f, -1.0f, 1.0f);
	viewM = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelM = glm::mat4(1.0f);
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

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
	colorProgarm = ShaderLoader::LoadShaders("Vertex.glsl", "Fragment.glsl", "");

	/*printf("Loading image..\n");
	backgroundWidth = BACKGROUND_WIDTH;
	backgroundHeight = BACKGROUND_HEIGHT;
	uint result = lodepng::decode(backgroundImageData, backgroundWidth, backgroundHeight, BACKGROUND_FILE_NAME);
	if (result != 0)
	{
		printf("Decode texture failed\n");
		return -1;
	}*/

	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//texture
	//glGenTextures(1, &backGroundId);
	//glBindTexture(GL_TEXTURE_2D, backGroundId);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, backgroundWidth, backgroundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &backgroundImageData[0]);
	
	//glUseProgram(colorProgarm);

	positionSlot = glGetAttribLocation(colorProgarm, "position");    //Binding vertex attribute slots to shader's input variables
	colorSlot = glGetAttribLocation(colorProgarm, "vertexColor");
	textureCoordSlot = glGetAttribLocation(colorProgarm, "textureCoord");
	matrixSlot = glGetUniformLocation(colorProgarm, "MVP");
	textureSamplerSlot = glGetUniformLocation(colorProgarm, "texUnit");

	glEnableVertexAttribArray(positionSlot);
	glEnableVertexAttribArray(colorSlot);
	glEnableVertexAttribArray(textureCoordSlot);

	UseShader(colorProgarm);

	//GLuint vertexBuffer;                //creating VBOs for vertex array and index array
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	//GLuint indexBuffer;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	glBufferData(GL_ARRAY_BUFFER, kTotalVerticesNumber * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);  //first buffers fill
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kTotalIndicesNumber * sizeof(GLushort), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 2));
	glVertexAttribPointer(textureCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GL_FLOAT)* 6));

	glUniform1i(textureSamplerSlot, 0);

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

void Renderer::UseShader(GLuint program)
{
	if (currentProgram != program)
	{
		if (currentVertexIndex > 0)
			DrawCurrentData();
		currentProgram = program;
		glUseProgram(currentProgram);
		glUniformMatrix4fv(matrixSlot, 1, GL_FALSE, &MVP[0][0]);
	}
}

void Renderer::Deinit()
{
	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(colorSlot);
	glDisableVertexAttribArray(textureCoordSlot);
	//glDisableVertexAttribArray(normalVectorSlot);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteProgram(colorProgarm);
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
	glBufferData(GL_ARRAY_BUFFER, kTotalVerticesNumber * sizeof(Vertex), vertices, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kTotalIndicesNumber * sizeof(GLushort), indices, GL_DYNAMIC_DRAW);

	glDrawElements(GL_TRIANGLE_STRIP, currentIndexIndex, GL_UNSIGNED_SHORT, 0);

	currentVertexIndex = 0;
	currentIndexIndex = 0;
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
	UseShader(colorProgarm);
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
	DrawCurrentData();

	// Swap buffers
	glfwSwapBuffers(window);


	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("GL ERROR: %s\n", gluErrorString(err));
	}
}