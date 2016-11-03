#include <random>
#include "renderengine.h"
#include <cmath>

#define SAMPLE 2

const Color RenderEngine::trace(const float i, const float j)
{
	Vector3D ray_dir = camera->get_ray_direction(i, j);
	Ray ray(camera->get_position(), ray_dir);
	return world->shade_ray(ray);
}

bool RenderEngine::renderLoop()
{
	static int i = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	for(int j = 0; j<camera->getHeight(); j++)
	{
		Color color(0);
		for(int p =0; p<SAMPLE; p++){
			for(int q=0; q<SAMPLE; q++){
				color = color + trace((const float) (i + (p + dis(gen)) / SAMPLE), (const float) (j + (q + dis(gen)) / SAMPLE));
			}
		}
		color = color / (SAMPLE*SAMPLE);
		color.clamp();
		camera->drawPixel(i, j, color);
	}

	if(++i == camera->getWidth())
	{
		i = 0;
		camera->incSteps();
		return false;
	}
	return false;
}
