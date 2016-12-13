//
//  ShaderLoader.m
//  ShaderViewer
//
//  Created by Max on 21/01/15.
//  Copyright (c) 2015 Max. All rights reserved.
//

#include "ShaderLoader.h"

//GLuint ShaderLoader::CompileShader(const char* shaderName, GLenum shaderType)
//{
//	std::ifstream f(shaderName);
//	if (f)
//	{
//		f.seekg(0, f.end);
//		const int fileSize = f.tellg();
//		f.seekg(0, f.beg);
//
//		char *buf = new char[fileSize];
//		f.read(buf, fileSize);
//		f.close();
//
//		GLuint shaderHandle = glCreateShader(shaderType);
//		glShaderSource(shaderHandle, 1, (const GLchar **)&buf, &fileSize);
//		glCompileShader(shaderHandle);
//
//		GLint compileSuccess;
//		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
//		if (compileSuccess == GL_FALSE)
//		{
//			GLchar messages[256];
//			glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
//			printf("Error during compiling shader: %s\n", messages);
//			return _UI32_MAX;
//		}
//
//		printf("... Ok!\n");
//
//		return shaderHandle;
//	}
//	else
//	{
//		printf("Error: unable to open file \"%s\"\n", shaderName);
//		return _UI32_MAX;
//	}
//}

GLuint ShaderLoader::CreateProgram(GLuint vertexHandle, GLuint fragmentHandle)
{
    GLuint programHandle = glCreateProgram();
    GLint vCompile, fCompile;
    glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &vCompile);
    glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &fCompile);
    if(vCompile == GL_FALSE || fCompile == GL_FALSE)
    {
        printf("\nCan't create shader program, invalid shader handle/s");
        return 0;
    }
    glAttachShader(programHandle, vertexHandle);
    glAttachShader(programHandle, fragmentHandle);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        return 0;
        //ToDo: print log error message here
        /*
         NSString *messageString = [NSString stringWithUTF8String:messages];
         NSLog(@"%@", messageString);
         exit(1);
         */
    }
    return programHandle;
}

void ShaderLoader::CompileShader(const char *fileName, GLuint *shaderId)
{
	// Read the Shader code from the file
	std::string shaderCode;
	std::ifstream shaderStream(fileName, std::ios::in);
	if (shaderStream.is_open()){
		std::string Line = "";
		while (getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fileName);
		getchar();
		return;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Shader
	printf("Compiling shader : %s\n", fileName);
	char const * VertexSourcePointer = shaderCode.c_str();
	glShaderSource(*shaderId, 1, &VertexSourcePointer, NULL);
	glCompileShader(*shaderId);

	// Check Vertex Shader
	glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(*shaderId, InfoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}
}

GLuint ShaderLoader::LoadShaders(const char * vertex, const char * fragment, const char * geometry)
{

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryShaderID = geometry == "" ? -1 : glCreateShader(GL_GEOMETRY_SHADER);

	CompileShader(vertex, &VertexShaderID);
	if (GeometryShaderID != -1)
	{
		CompileShader(geometry, &GeometryShaderID);
	}
	CompileShader(fragment, &FragmentShaderID);

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	if (GeometryShaderID != -1)
	{
		glAttachShader(ProgramID, GeometryShaderID);
	}
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	if (GeometryShaderID != -1)
	{
		glDeleteShader(GeometryShaderID);
	}

	return ProgramID;
}

GLuint ShaderLoader::LoadComputeShader(const char* computeShaderFile)
{
	GLuint computeShaderId = glCreateShader(GL_COMPUTE_SHADER);

	CompileShader(computeShaderFile, &computeShaderId);
	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, computeShaderId);
	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	glDeleteShader(computeShaderId);
	return ProgramID;
}