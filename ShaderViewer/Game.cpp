#include "Game.h"
#include "Lodepng.h"

Game::Game()
{
	renderer = std::make_shared<Renderer>();
}

Game::~Game()
{

}

int Game::Init()
{
	int res = renderer->Initialize();
	if (res != 0)
		return res;
	images.clear();
	textures.clear();
	GLuint texId = loadTexture("map.png", BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
	if (texId == 0)
		return -2;
	auto img = std::make_shared<RImage>(0, 0, 0.0f, 0.0f, 1.0f, 1.0f, RENDER_WIDTH, RENDER_HEIGHT, texId);
	images.push_back(std::pair<const char*, std::shared_ptr<RImage>>("background", img));

	return 0;
}

GLuint Game::loadTexture(const char* textureFile, uint width, uint height)
{
	std::vector<unsigned char> texData;
	GLuint res = lodepng::decode(texData, width, height, textureFile);
	if (res != 0)
		return 0;
	glGenTextures(1, &res);
	glBindTexture(GL_TEXTURE_2D, res);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texData[0]);
	textures.push_back(res);
	return res;
}

void Game::Deinit()
{
	renderer->Deinit();
	glDeleteTextures(textures.size(), &textures[0]);
}

void Game::Update(float dt)
{
	renderer->SimulateParticles();
}

void Game::Draw()
{
	//renderer->DrawSprite(images[0].second, 0.0f, 0.0f);

	renderer->OnDraw();
}