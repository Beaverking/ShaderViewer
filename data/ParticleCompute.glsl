#version 450 core

uniform float gravity;
uniform float dt;
uniform vec2 renderSize;

struct Particle
{
	vec2 position;
	vec2 velocity;
	vec4 color;
};

layout (local_size_x = 90, local_size_y = 1) in;
layout (std430, binding = 2) buffer particleBuffer
{
	Particle particle_array[];
};

void main(void)
{
	uint idx = gl_GlobalInvocationID.x;
	Particle part = particle_array[idx];
	part.position.x += part.velocity.x * dt;
	part.position.y += (part.velocity.y + gravity) * dt;
	if (part.position.x < 0.0)
	{
		part.position.x = 0.0;
		part.velocity.x = -part.velocity.x;
	}
	else if (part.position.x > renderSize.x)
	{
		part.position.x = renderSize.x;
		part.velocity.x = -part.velocity.x;
	}
	if (part.position.y < 0.0)
	{
		part.position.y = 0.0;
		part.velocity.y = -part.velocity.y;
	}
	else if (part.position.y > renderSize.y)
	{
		part.position.y = renderSize.y;
		part.velocity.y = -part.velocity.y;
	}
	particle_array[idx] = part;
}