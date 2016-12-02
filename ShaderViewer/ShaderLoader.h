//
//  ShaderLoader.h
//  ShaderViewer
//
//  Created by Max on 21/01/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#ifndef ShaderViewer_ShaderLoader_h
#define ShaderViewer_ShaderLoader_h

#include "Common.h"

class ShaderLoader
{
public:
    //static GLuint CompileShader(const char* shaderName, GLenum shaderType);
    static GLuint CreateProgram(GLuint vertexHandle, GLuint fragmentHandle);
	static GLuint LoadShaders(const char * vertex, const char * fragment, const char * geometry);

	static void CompileShader(const char *fileName, GLuint *shaderId);

};

#endif
