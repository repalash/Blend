//quadric.h
#ifndef _QUADRIC_H_
#define _QUADRIC_H_


#include <iostream>
using namespace std;
#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"
#include "lightsource.h"
#include <cmath>

#define degreeToRadians(X) ((X)*M_PI/180.0f)

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif

class Quadric : public Object
{
private:
	double A, B, C, D, E, F, G, H, I, J;
	bool out;
	Vector3D position;
	double radius;

public:

	Quadric(const Vector3D& _pos, double A, double B, double C, double D, double E, double F, double G, double H, double I,
	        double J, bool out, Material* mat) : position(_pos), Object(mat), A(A), B(B), C(C), D(D), E(E), F(F), G(G), H(H), I(I), J(J), out(out) {
		isSolid = true;
		radius = sqrt(1.0/A);
		lightSource = nullptr;
		drawShape(mat);
	}

	virtual bool intersect(Ray& r) const;
	void drawShape(Material* mat);
	virtual Vector3D getPosition() const { return position; };
	void changePosition(Material* mat, Vector3D dtrans) {
		position = position + dtrans;
		cout << position[0] << " " << position[1] << " " << position[2] << endl;
		drawShape(mat);
	};
	void changeRadius(Material* mat, float scaleF) {
		if (radius + scaleF > 1.0) {
			radius = radius+scaleF;
			drawShape(mat);
		}
	};
};
#endif
