#include <vector>
#include "world.h"
#include "material.h"
#include <iostream>
#include <cmath>
#include <random>

unsigned int x=123456789, y=362436069, z=521288629;
float xorshf96() {          //3.6 times better performance than rand()
//	return 1.f*rand()/RAND_MAX;
	unsigned int t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;
	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;
	return 1.0f*z/0xffffffff;
}

Vector3D randomRotate(Vector3D vector, float angle){
	angle = (rand()%360)*angle/180-angle; angle = angle*M_PI/180;
	Vector3D randVector(1+(rand()%100), 1+(rand()%100), 1+(rand()%100));
	Vector3D k = crossProduct(randVector, vector);
	return Vector3D(vector*cos(angle) + crossProduct(k, vector)*sin(angle) + k*dotProduct(k, vector)*(1-cos(angle))); //Rodrigue's rotation formula
}

Color Material::shade(const Ray& incident, const bool isSolid) const
{
	if(!incident.didHit()) return world->getBackground();

	if(incident.intersected()->isLightSource() &&
	   xorshf96()<incident.intersected()->getLightSource()->getIntensity().maxComponent())  //randomly determine to emit light
		return incident.intersected()->getLightSource()->getIntensity();

	Color finalColor(color);

	double cosTheta = dotProduct(incident.getDirection(), incident.getNormal());
	Vector3D rDirection;
	rDirection = incident.getDirection() - 2 * incident.getNormal() * cosTheta;
	rDirection.normalize();
	Ray reflectedRay = Ray(incident.getPosition(), rDirection, incident.getLevel() + 1);  //Ideal reflection

	if(kt>0){
		//material is dielectric
		bool isInside = cosTheta > 0;
		double nc=1, nnt=isInside?eta/nc:nc/eta;
		double cos2t = 1-nnt*nnt*(1-cosTheta*cosTheta);
		if (cos2t<0){ //TIR
			return finalColor*world->shade_ray(reflectedRay);
		}else{
//			std::cout<<"asdasd"<<std::endl;
			cosTheta = -fabs(cosTheta);
			Vector3D tDirection = (incident.getDirection()*nnt - incident.getNormal()*((isInside?-1:1)*(cosTheta*nnt+sqrt(cos2t))));
			Ray refractedRay = Ray(incident.getPosition(), tDirection, incident.getLevel()+1);

			double a=eta-nc, b=eta+nc, R0=a*a/(b*b), c = 1-(isInside?dotProduct(tDirection, incident.getNormal()):-cosTheta);
			double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);

			if(incident.getLevel()>1)
				if (xorshf96() < P) return finalColor * world->shade_ray(reflectedRay) * RP;
				else return finalColor * world->shade_ray(refractedRay) * TP;
			return finalColor * (world->shade_ray(reflectedRay)*Re + world->shade_ray(refractedRay)*Tr);
		}
	}else if(n>0)
	{
		double phi=2*M_PI*xorshf96(),
				cosAlpha=pow(xorshf96(), 1.f/(n+1)), sineAlpha = sqrt(1-cosAlpha*cosAlpha);
		double rotAngle = 2*(acos(-cosTheta) + acos(cosAlpha) - M_PI/2);

		Vector3D w=rDirection;
		Vector3D u=(crossProduct((fabs(w.X())>.1?Vector3D(0,1,0):Vector3D(1,0,0)),w));
		u.normalize();
		Vector3D v=crossProduct(w,u);

		Vector3D dDirection = u*cos(phi)*sineAlpha + v*sin(phi)*sineAlpha + w*cosAlpha;

		if(dotProduct(dDirection,incident.getNormal())<0) {
			Vector3D k = crossProduct(rDirection, incident.getNormal());
			k.normalize();
			dDirection = cos(rotAngle) * dDirection + sin(rotAngle) * crossProduct(k, dDirection);
		}
		Ray randomRay = Ray(incident.getPosition(), dDirection, incident.getLevel() + 1);  //Ideal reflection
		return finalColor*world->shade_ray(randomRay);
	}
	else if(!kr || xorshf96()>kr){ //kr=1 means completely reflective
		//do diffuse, Lambertian sampling
		double alpha=2*M_PI*xorshf96(),
				z=xorshf96(), sineTheta = sqrt(1-z);


		//generate basis
		Vector3D w=incident.getNormal();
		Vector3D u=(crossProduct((fabs(w.X())>.1?Vector3D(0,1,0):Vector3D(1,0,0)),w));
		u.normalize();
		Vector3D v=crossProduct(w,u);
		Vector3D dDirection = u*cos(alpha)*sineTheta + v*sin(alpha)*sineTheta + w*sqrt(z) ;
		Ray randomRay = Ray(incident.getPosition(), dDirection, incident.getLevel() + 1);  //Ideal reflection
		if(kg>0)
			return finalColor*world->shade_ray(randomRay) *(kg * pow(dotProduct(rDirection, dDirection), n)) * dotProduct(w, dDirection); //Glossy
		else
			return finalColor*world->shade_ray(randomRay);
	}else{
		//do specular
		return finalColor*world->shade_ray(reflectedRay);
	}
	return finalColor;

	const std::vector<LightSource *> &lights = world->getLightSourceList();
	bool isInside = cosTheta > 0;
	if(isInside) cosTheta = dotProduct(incident.getDirection(), -incident.getNormal());

	float lobe = (float) (kr * kr);
	Color reflectedColor(0);
	float wtSum = 0;

	if(rand()%3)
	{
		reflectedColor = lobe*world->shade_ray(reflectedRay); wtSum = lobe;
	}
	for(int i=0;i<kr*4;i++) {
		Vector3D randDirection = randomRotate(incident.getNormal(), 45);
		lobe = (float) pow(std::max(dotProduct(randDirection, incident.getNormal()), 0.), n);
		Ray _reflectedRay = Ray(incident.getPosition(), randDirection,
		                        incident.getLevel() + 1);
		reflectedColor = reflectedColor + lobe*world->shade_ray(_reflectedRay);
		wtSum += lobe;
	}
	reflectedColor = reflectedColor / wtSum;
	double reflectance = pow((eta-1)/(eta+1), 2);
	double schlickApp = reflectance + (1 - reflectance) * pow(1 + cosTheta, 1);
	if(isSolid && kt && eta && incident.getLevel() < MAX_LEVEL){ //Do refraction
		double currentEta = isInside?1:eta;
		Vector3D direction = incident.getRefractive_index() * (incident.getDirection() - incident.getNormal()*cosTheta) / currentEta;
		direction = direction - incident.getNormal() * sqrt(1-(incident.getRefractive_index()*incident.getRefractive_index()*(1-cosTheta*cosTheta)/(currentEta*currentEta)));
		direction.normalize();
		Ray refractedRay = Ray(incident.getPosition(), direction, incident.getLevel()+1, (float) currentEta);
		Color beerK(1);
		if(isInside) {
			world->firstIntersection(refractedRay);
			double t = (refractedRay.getPosition() - incident.getPosition()).length()/5;
			beerK.r = exp(-color.r*t);
			beerK.g = exp(-color.g*t);
			beerK.b = exp(-color.b*t);
		}
		finalColor = finalColor + ((kr * schlickApp) * reflectedColor + beerK * kt * world->shade_ray(refractedRay));
	}else {
		finalColor = finalColor + (kr + schlickApp) * reflectedColor;
	}
	finalColor.clamp();
	finalColor = finalColor * color;
	Color myColor = color + finalColor;
	myColor.clamp();
	for(int i=0; i<lights.size(); i++){
		LightSource* light = lights[i];
		Vector3D l = incident.getPosition() - light->getPosition();
		Vector3D r = l - 2 * incident.getNormal() * dotProduct(l, incident.getNormal());

		Ray shadowRay = Ray(incident.getPosition(), -l, incident.getLevel()+1);
		world->firstIntersection(shadowRay);
		if(!shadowRay.didHit() || shadowRay.intersected()->getLightSource() != light) continue;

		double attenuation = 1.0f / (1.0f + katt * pow(l.length(), 2));
		r.normalize();
		l.normalize();

		Color diffuse = myColor * light->getIntensity() * std::max( dotProduct(incident.getNormal(), -l), 0.0);
		Color specular = light->getIntensity() * pow( std::max(dotProduct(r, -incident.getDirection()), 0.0), n);
		finalColor = finalColor + attenuation * (kd * diffuse + ks * specular);

		finalColor.clamp();
	}
	return finalColor;
}
