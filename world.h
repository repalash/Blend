#ifndef _WORLD_H_
#define _WORLD_H_

#define MAX_LEVEL 10

#include <vector>
#include "object.h"
#include "lightsource.h"
#include "shader_utils.h"

class World
{
private:
	std::vector<Object*> objectList;
	std::vector<LightSource*> lightSourceList;

	Color ambient;
	Color background; //Background color to shade rays that miss all objects

public:
	World():
		objectList(0), lightSourceList(0), ambient(0), background(0)
	{}
	void setBackground(const Color& bk) { background = bk;}
	Color getBackground() { return background;}
	void setAmbient(const Color& amb) {ambient = amb;}
	Color getAmbient() {return ambient;}

	void addLight(LightSource* ls);
	void addObject(Object *obj)
	{
		objectList.push_back(obj);
	}
	float firstIntersection(Ray& ray);
	int findIntersection(Ray& r1);
	Color shade_ray(Ray& ray);
	const std::vector<LightSource *> &getLightSourceList() const { return lightSourceList; }

	void drawObjects(GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib);
	void translateObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF, int axisBox);
	void scaleObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF);
	void changeColor(int pos, Color nc);
};

#endif
