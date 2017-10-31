#include <random>
#include "renderengine.h"
#include <iostream>
using namespace std;

const Color RenderEngine::trace(const float i, const float j)
{
	Vector3D ray_dir = camera->get_ray_direction(i, j);
	Ray ray(camera->get_position(), ray_dir);
	return world->shade_ray(ray);
}

bool RenderEngine::renderLoop()
{
	// static int i = 0;
#pragma omp parallel for schedule(guided, 4)
	for(int j = 0; j<camera->getHeight(); j++)
	{
		Color color(0);
		for(int p =0; p<SAMPLE; p++){
			for(int q=0; q<SAMPLE; q++){
				color = color + trace((const float) (startPos + (p + xorshf96()) / SAMPLE), (const float) (j + (q + xorshf96()) / SAMPLE));
			}
		}
		color = color / (SAMPLE*SAMPLE);
		color.clamp();
		camera->drawPixel(startPos, j, color);
	}
	if(++startPos == camera->getWidth())
	{
		startPos = 0;
		camera->incSteps();
		std::cout<<"Samples Done: "<<camera->getSteps()*SAMPLE*SAMPLE<<std::endl;
		return false;
	}
	return false;
}

void RenderEngine::renderObjects(GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib) {
	world->drawObjects(vVertex_attrib, vColor_attrib, vNormal_attrib);
}

pair <int, float> RenderEngine::checkIntersect(Vector3D e, Vector3D s) {
	Ray r1(e, s-e);
	return make_pair(world->findIntersection(r1), r1.getParameter());
}

void RenderEngine::translateObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF, int axisPos) {
	Ray r1(oldC, oldF-oldC);
	world->translateObject(pos, t0, oldC, newC, oldF, newF, axisPos);
}

void RenderEngine::scaleObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF) {
	Ray r1(oldC, oldF-oldC);
	world->scaleObject(pos, t0, oldC, newC, oldF, newF);
}
