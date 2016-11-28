#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include "world.h"
#include "camera.h"
#include "material.h"
#include <iostream>
using namespace std;

class RenderEngine
{
private:
	World *world;
	Camera *camera;
	const Color trace(const float i, const float j);
	bool renderFlag;
	int SAMPLE;
	int startPos;

public:
	RenderEngine(World *_world, Camera *_camera, int _SAMPLE):
		world(_world), camera(_camera), SAMPLE(_SAMPLE) {
			startPos = 0;
		}
	void setStartPos(int x) { startPos = x;};
	bool renderLoop();
	void setRenderFlag(bool flag) { renderFlag = flag;};
	bool getRenderFlag() { return renderFlag; };
	void renderObjects(GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib);
	pair < int, float > checkIntersect(Vector3D, Vector3D);
	void translateObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF, int axisPos);
	void scaleObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF);
	void setSamples(int _nsample) { SAMPLE = _nsample; };
	void changeColor(int pos, Color nc) { world->changeColor(pos, nc); };
	void changeMaterial(int pos, float kr, int kt, float eta, int phongn) { world->changeMaterial(pos, kr, kt, eta, phongn); };
};
#endif
