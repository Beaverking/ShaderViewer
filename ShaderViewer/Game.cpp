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

	explosionTextureData.resize(EXPLOSION_RAD * EXPLOSION_RAD);
	memset(&explosionTextureData[0], 0, explosionTextureData.size());
	float explRadSq = EXPLOSION_RAD * EXPLOSION_RAD;
	int r = static_cast<int>(floorf(EXPLOSION_RAD) + 0.5f);
	int startX = 0, startY = 0;
	int centerX = EXPLOSION_RAD, centerY = EXPLOSION_RAD;
	int curY = startY;
	while (curY < startY + (r * 2))
	{
		for (int curX = startX; curX < startX + (r * 2); ++curX)
		{
			int currDistSq = (curX - centerX) * (curX - centerX) + (curY - centerY) * (curY - centerY);
			if (currDistSq > explRadSq)
				continue;
			explosionTextureData[curY * EXPLOSION_RAD * 2 + curX] = 255;
		}
		curY++;
	}

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
	if (updateExplosionTex)
	{
		renderer->SetTexture(explosionTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, &explosionTextureData[0]);
		updateExplosionTex = false;
	}
	//renderer->DrawSprite(images[0].second, 0.0f, 0.0f);
	renderer->DrawSprite(explosionImg, 0.0f, 0.0f);

	renderer->OnDraw();
}

void Game::OnLbMouseClick(float x, float y)
{
	
	updateExplosionTex = true;
}