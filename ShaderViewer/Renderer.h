//
//  Renderer.h
//  ShaderViewer
//
//  Created by Max on 21/07/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#ifndef __ShaderViewer__Renderer__
#define __ShaderViewer__Renderer__

#include "Common.h"

const int kTotalVerticesNumber = 5000;      //vertex array max size. If number of verts per frame to render > this value, frame will be rendered in multiple passes
const int kTotalIndicesNumber = kTotalVerticesNumber * 3;

const static Color4f kDebugCircleColorDefault = Color4f(1.0f, 0.4f, 0.4f, 0.7f);
const static Color4f kDebugCircleColorEnemy = Color4f(1.0f, 0.4f, 0.4f, 0.7f);
const static Color4f kDebugCircleColorPlayer = Color4f(0.2f, 0.8f, 0.5f, 0.7f);

const static Color4f kDebugPolyColorDefault = Color4f(0.4f, 1.0f, 0.4f, 0.7f);
const static Color4f kDebugPolyColorLevel = Color4f(0.5f, 0.6f, 0.5f, 0.7f);
const static Color4f kDebugPolyColorActivator = Color4f(0.6f, 0.2f, 0.8f, 0.7f);

class Renderer
{
public:
    Renderer();
    ~Renderer();

	enum ShadeMode
	{
		kSolidColor = 0,
		kTexture
	};
    
    int Initialize();

	const Vec2f GetFrameSize();

	void DrawSolidQuad(float x, float y, float x1, float y1, const Color4f &col);
	void DrawSolidQuad(float x, float y, int width, int height, const Color4f &col, float angle = 0.0f);
	void DrawSprite(ImageP img, float x, float y, float a = 1.0f, bool centered = false, float angle = 0.0f, float scaleVert = 1.0f, float scaleHoriz = 1.0f);


	void Deinit();
    void OnDraw();

	GLFWwindow * GetRenderWindow() const { return window; }
    
    GLfloat *GetMVPPointer() { return &MVP[0][0]; }

private:
	void SetTexture(int tIndex);
	void SetShadeMode(ShadeMode mode);
	void GetQuadVertices(Vec2f *verts, const ImageP img, float angle, float scaleX, float scaleY);
	void SetVertexQuadData(Vec2f *positions, Vec2f *uvs, const Color4f &color);

	Vertex          *vertices;
	Color4f         bufColor;
	GLushort        *indices;

	int     currentVertexIndex;
	int     currentIndexIndex;
	int     width;
	int     height;
	int     currentTextureIndex;
	int     currentShadeMode;

    glm::mat4 projectionM;
    glm::mat4 viewM;
    glm::mat4 modelM;
    glm::mat4 MVP;
    
    GLuint      colorProgarm;
	GLuint      currentProgram;
    
    GLuint      positionSlot;
	GLuint		colorSlot;
	GLuint		textureCoordSlot;
	GLuint		textureSamplerSlot;

	GLuint      matrixSlot;
    
    GLuint      indexBufferId;
    GLuint      vertexBufferId;
	GLuint		vertexArrayId;

	GLFWwindow* window;	//main render window
};

#endif /* defined(__ShaderViewer__Renderer__) */
