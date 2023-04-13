#include "Effect.h"
#include <random>
#include <time.h>

Effect::Effect() :	Effect(20,2,0,0,0,0,0,0,0,0,0,0) {}

Effect::Effect(int size, float lifetime,float x, float y, int minDistX, int maxDistX, int minDistY, int maxDistY, int minVelX, int maxVelX, int minVelY, int maxVelY)
{
	srand(time(NULL));
	_size = size;
	_texture = nullptr;
	_lifetime = lifetime;
	for (size_t i = 0; i < _size; i++)
	{
		Particle p = {
			SDL_Rect(),
			0,
			x + minDistX + rand() % (maxDistX - minDistX + 1),
			y + minDistY + rand() % (maxDistY - minDistY + 1),
			minVelX + rand() % (maxVelX - minVelX + 1),
			minVelY + rand() % (maxVelY - minVelY + 1),
			0,
			(rand() % 30) - 15,
			110 + rand() % (255 - 110 + 1)
		};
 
		p.rect.w = 30;
		p.rect.h = 30;
		p.rect.x = x + (rand() % (maxDistX - minDistX + 1));
		p.rect.y = y + (rand() % (maxDistY - minDistY + 1));
		particles.push_back(p);
	}
}

Effect::~Effect() 
{

}

void Effect::SetTexture(SDL_Texture* texture)
{
	_texture = texture;
}

void Effect::draw()
{
	for (size_t i = 0; i < _size; i++)
	{
		SDL_SetTextureAlphaMod(_texture, particles[i].alpha);
		//SDL_RenderCopy(_renderer, _texture, NULL, &_particles[i].rect);
		SDL_RenderCopyEx(_renderer, _texture, nullptr, &particles[i].rect, particles[i].rotation, nullptr, SDL_FLIP_NONE);
		//SDL_RenderCopyEx(_renderer, _texture, NULL, &_particles[i].rect);

	}
}

void Effect::init(SDL_Renderer* renderer)
{
	_renderer = renderer;
}

void Effect::update(float deltaTime)
{
	_deltaTime = deltaTime;

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].lifetime += deltaTime;
		if (particles[i].lifetime > _lifetime)
		{
			particles.erase(particles.begin() + i);
			_size--;
		}
		continue;
	}
	move();
	transparency();
	rotation();
}

void Effect::move()
{
	srand(time(NULL));
	float x = 0;
	float y = 0;
	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i].virtualX += particles[i].velX * _deltaTime;
		particles[i].virtualY += particles[i].velY * _deltaTime;

		particles[i].rect.x = (int)particles[i].virtualX;
		particles[i].rect.y = (int)particles[i].virtualY;
	}
}

void Effect::transparency() {

	for (size_t i = 0; i < particles.size(); i++)
	{
		_transparencyVelocity = (_deltaTime * particles[i].alpha) / (_lifetime - particles[i].lifetime);
		if (particles[i].alpha - _transparencyVelocity <= 0)
			particles[i].alpha = 0;
		else
			particles[i].alpha -= _transparencyVelocity;
	}
}
void Effect::rotation() {

	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i].rotation += particles[i].rotationSpeed * _deltaTime;
	}
}