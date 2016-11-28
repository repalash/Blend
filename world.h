#ifndef _WORLD_H_
#define _WORLD_H_

#define MAX_LEVEL 20

#include <vector>
#include "object.h"
#include "lightsource.h"
#include "color.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include "quadric.h"
#include "ray.h"

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
	Object * firstIntersection(Ray& ray);
	int findIntersection(Ray& r1);
	Color shade_ray(Ray& ray);
	const std::vector<LightSource *> &getLightSourceList() const { return lightSourceList; }

	void drawObjects(GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib);
	void translateObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF, int axisBox);
	void scaleObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF);
	void changeColor(int pos, Color nc) { 
		objectList[pos]->getMaterial()->color.r = nc.r;
		objectList[pos]->getMaterial()->color.g = nc.g;
		objectList[pos]->getMaterial()->color.b = nc.b;
		if ((int)objectList[pos]->getExtendedVertices().size() == 93276) {
			(dynamic_cast<Sphere *>(objectList[pos]))->drawShape(objectList[pos]->getMaterial());
		}
		else if ((int)objectList[pos]->getExtendedVertices().size() == 9) {
			(dynamic_cast<Triangle *>(objectList[pos]))->drawShape(objectList[pos]->getMaterial());
		}
		else if ((int)objectList[pos]->getExtendedVertices().size() == 6444) {
			(dynamic_cast<Quadric *>(objectList[pos]))->drawShape(objectList[pos]->getMaterial());
		}
		else {
			
		}
	}
};

#endif
