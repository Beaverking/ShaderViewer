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

//const int kMaxInices = MAX_VERTS * 1.5f;

class Renderer
{
public:
    Renderer();
    ~Renderer();
    
    int Initialize();
	void Deinit();
    void OnDraw();

	GLFWwindow * GetRenderWindow() const { return window; }
    
    GLfloat *GetMVPPointer() { return &MVP[0][0]; }

private:
    glm::mat4 projectionM;
    glm::mat4 viewM;
    glm::mat4 modelM;
    glm::mat4 MVP;
    
    GLuint      colorProgarm;
    
    GLuint      hRenderBuffer;
    GLuint      positionSlot;
    GLuint      normalVectorSlot;
	GLuint		textureCoordSlot;
	GLuint		textureSamplerSlot;
	GLuint		sceneFlagSlot;

	GLuint      matrixSlot;
	GLuint		viewMatrixSlot;
	GLuint		modelMatrixSlot;
	GLuint		lightPositionSlot;
	GLuint		samplerSlot;
    
    GLuint      indexBufferId;
    GLuint      vertexBufferId;
	GLuint		vertexArrayId;

	//Depth buffer stuff
	GLuint		depthBufferId;
	GLuint		depthTextureId;
	GLuint		depthTextureSlot;


	Vertex quadVerts[6];	//vertices used in render to texture;
    
    

	GLFWwindow* window;	//main render window

	std::vector<unsigned char> backgroundImageData;
	uint backgroundWidth;
	uint backgroundHeight;
	GLuint backGroundId;

	float   projectionMatrix[16];
};

#endif /* defined(__ShaderViewer__Renderer__) */
