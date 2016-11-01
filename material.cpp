#include <vector>
#include "world.h"
#include "material.h"
#include <iostream>
Color Material::shade(const Ray& incident, const bool isSolid) const
{
	if(!incident.didHit()) return world->getBackground();
	const std::vector<LightSource *> &lights = world->getLightSourceList();
	Color finalColor(ka*world->getAmbient()*color);
	double cosTheta = dotProduct(incident.getDirection(), incident.getNormal());
	bool isInside = cosTheta > 0;
	if(isInside) cosTheta = dotProduct(incident.getDirection(), -incident.getNormal());
	Ray reflectedRay = Ray(incident.getPosition(), incident.getDirection() - 2 * incident.getNormal() * cosTheta, incident.getLevel() + 1);
	Color reflectedColor = world->shade_ray(reflectedRay);

	double reflectance = pow((eta-1)/(eta+1), 2);
	double schlickApp = reflectance + (1 - reflectance) * pow(1 + cosTheta, 5);
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
		finalColor = finalColor + ((kr + schlickApp) * reflectedColor + beerK * kt * world->shade_ray(refractedRay));
	}else {
		finalColor = finalColor + (kr * schlickApp) * reflectedColor;
	}
	Color myColor = color + finalColor;
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
