#pragma once
#include "SDL.h"
#include <vector>

struct Particle {
	SDL_Rect rect;
	float lifetime;
	float virtualX;
	float virtualY;
	float velX;
	float velY;
	float rotation;
	float rotationSpeed;
	float alpha;
};

class Effect {

public:
	Effect();
	Effect(int size, float lifetime, float x, float y, int minDistX, int maxDistX, int minDistY, int maxDistY, int minVelX, int maxVelX, int minVelY, int maxVelY);
	~Effect();
	void draw();
	void init(SDL_Renderer* renderer);
	void update(float deltaTime);
	void move();
	void transparency();
	void rotation();
	void SetTexture(SDL_Texture* texture);
	std::vector<Particle> particles;
private:
	int _size;
	SDL_Renderer* _renderer;
	SDL_Texture* _texture;
	float _deltaTime;
	float _startVelocity;
	float _decreaseVelocity;
	float _lifetime;
	float _transparencyVelocity;
	float _rotationSpeed = 10;
	float timepassed;

};

