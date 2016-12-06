#ifndef __ShaderViewer__Game__
#define __ShaderViewer__Game__

#include "Renderer.h"
#include <map>

class Game
{
public:
	Game();
	~Game();

	int Init();
	void Deinit();
	void Update(float dt);
	void Draw();

	std::shared_ptr<Renderer> GetRenderer() const { return renderer; }

private:
	GLuint loadTexture(const char* textureFile, uint width, uint height);


	std::shared_ptr<Renderer> renderer;
	std::vector<std::pair<const char*, std::shared_ptr<RImage>>> images;
	std::vector<GLuint> textures;

	Vec2f pos;
};

#endif