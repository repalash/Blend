//object.h
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include "ray.h"
#include "vector3D.h"
#include "color.h"
#include "material.h"
#include "lightsource.h"

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif

class Object
{
protected:
	Material *material;
	bool isSolid;
	LightSource* lightSource;
	bool isMovement;

	std::vector < GLfloat > expandedVertices; 
	std::vector < GLfloat > expandedColors; 
	std::vector < GLfloat > expandedNormals; 

public:
	Object(Material *mat): material(mat) {}	
	virtual bool intersect(Ray& ray) const = 0;
	virtual Color shade(const Ray& ray) const
	{
		return material->shade(ray, isSolid);
	}
	virtual Vector3D getPosition() const {
		return Vector3D(0, 0, 0);
	}
	void setLightSource(LightSource *ls){ lightSource = ls; }
	bool isLightSource() const { return lightSource!=nullptr; };
	const LightSource* getLightSource() const { return lightSource; };

	std::vector < GLfloat > getExtendedVertices() { return expandedVertices; }
	std::vector < GLfloat > getExtendedColors() { return expandedColors; }
	std::vector < GLfloat > getExtendedNormals() { return expandedNormals; }
	Material* getMaterial() { return material;}
	bool getIsMovement() { return isMovement; };
	// virtual void foo() {};
};

#endif
