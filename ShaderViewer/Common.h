//
//  Common.h
//  ShaderViewer
//
//  Created by Max on 21/07/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#ifndef ShaderViewer_Common_h
#define ShaderViewer_Common_h

#define RENDER_WIDTH 1024
#define RENDER_HEIGHT 768
#define BACKGROUND_WIDTH 256u
#define BACKGROUND_HEIGHT 192u
#define MS_PER_FRAME 16

#define BACKGROUND_FILE_NAME "map.png"

#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>

typedef unsigned int uint;

//vertex structrure used to render stuff
struct Vertex
{
    Vertex(): x(0), y(0), u(0), v(0) {}
    Vertex(GLfloat _x, GLfloat _y, GLfloat _u, GLfloat _v)
    : x(_x), y(_y),
	u(_u), v(_v) {};
    
	GLfloat x, y;			//position
	GLfloat u, v;			//texture coords
};

inline unsigned int GetCurrentMS()
{
	return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

#endif
