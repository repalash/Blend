//matrial.h
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "color.h"
#include "ray.h"

class World;

class Material
{
private:
	World *world;
public:
	//Data

	Color color; // color numbers
	double ka;//Ambient Contribution
	double kd;//Diffuse constant
	double ks;//Specular constant
	double kr;//Contribution from reflection, 0 <= kr <= 1
	double kt;//Contribution from refraction, boolean
	double katt;//Attenuation factor,
	double eta;//Coefficient of refraction, single float
	double n;//Phong's glossy constant, 0 <= n <= inf
	// kg : boolean, if n == 0 kg false else true
	// number of samples
	Material(World *w):
		world(w), color(0),
		ka(0), kd(0.0), ks(0), kr(0), kt(0),n(0), eta(0) {}
	Color shade(const Ray& incident, const bool isSolid) const;

};


float xorshf96();

#endif
