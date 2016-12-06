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
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>

typedef unsigned int uint;
const static float PI = 3.14159265359f;

//vertex structrure used to render stuff
struct Vertex
{
    Vertex(): x(0), y(0), r(1.0f), g(1.0f), b(1.0f), u(0), v(0) {}
    Vertex(GLfloat _x, GLfloat _y, GLfloat _u, GLfloat _v)
    : x(_x), y(_y),
	u(_u), v(_v) {};
    
	GLfloat x, y;			//position
	GLfloat r, g, b, a;     //color
	GLfloat u, v;			//texture coords
};

struct Color4f
{
	Color4f() : r(0), g(0), b(0), a(1.0f) {};
	Color4f(float r, float g, float b, float a)
		: a(std::min(std::max(0.0f, a), 1.0f))
		, r(std::min(std::max(0.0f, r), 1.0f))
		, g(std::min(std::max(0.0f, g), 1.0f))
		, b(std::min(std::max(0.0f, b), 1.0f))
	{};
	Color4f(float r, float g, float b)
		: a(1.0f)
		, r((std::min(std::max(0.0f, r), 1.0f)))
		, g((std::min(std::max(0.0f, g), 1.0f)))
		, b((std::min(std::max(0.0f, b), 1.0f)))
	{};
	Color4f(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : a(a), r(r), g(g), b(b) {};
	//0xRRGGBBAA
	Color4f(uint32_t c) : a(((c << 24) >> 24) / 255.0f), r((c >> 24) / 255.0f), g(((c << 8) >> 24) / 255.0f), b(((c << 16) >> 24) / 255.0f) {};  //0xffffffff

	float r, g, b, a;
};

struct Vec2f
{
	Vec2f() : x(0), y(0) {};
	Vec2f(float x, float y) : x(x), y(y) {};
	Vec2f(int x, int y) : x(float(x)), y(float(y)) {};
	Vec2f operator +(const Vec2f &v) const { return Vec2f(x + v.x, y + v.y); };
	Vec2f operator -(const Vec2f &v) const { return Vec2f(x - v.x, y - v.y); };
	Vec2f operator *(const float &f) const { return Vec2f(x * f, y * f); };
	Vec2f operator /(const float &f) const { return Vec2f(x / f, y / f); };
	float MagnitudeSquared() const { return (x * x + y * y); }
	float Magnitude() const { return sqrtf(MagnitudeSquared()); }
	Vec2f  &Normalize() { float inv = 1.0f / sqrtf(x * x + y * y); x *= inv; y *= inv; return *this; }
	void LimitMagnitude(const float r)
	{
		if (Magnitude() > r && Magnitude() > 0.0f)
		{
			float rat = r / Magnitude();
			x *= rat;
			y *= rat;
		}
	};

	float x;
	float y;
};

struct RImage
{
	RImage() : x(0), y(0), u(0), u1(0), v(0), v1(0), w(0), h(0), pivotX(-1), pivotY(-1)
	{}
	RImage(int x, int y, float u, float v, float u1, float v1, int w, int h, unsigned int textureID)
		: x(x), y(y)
		, u(u), v(v)
		, u1(u1), v1(v1)
		, w(w), h(h)
		, tID(textureID)
		, pivotX(-1)
		, pivotY(-1)
	{}

	void SetPivot(int x, int y) { pivotX = x; pivotY = y; }

	unsigned int tID;     //id of texture in opengl
	int x, y;             //position in texture
	float u, u1, v, v1;   //texture coordinates
	int w, h;             //height and width of rect with image data
	int pivotX, pivotY;   //image's pivot point
};

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

typedef std::shared_ptr<RImage> ImageP;

inline unsigned int GetCurrentMS()
{
	return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

#endif
