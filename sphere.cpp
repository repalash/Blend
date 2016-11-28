//sphere.cpp

#include "sphere.h"
#include "lightsource.h"

void Sphere::drawShape(Material* mat) {
	const int nVertices = 3*72*72, exnVertices = 71*72*2*3 + 3*70 + 3*70;
	float sphere_vertices[nVertices];
	float sphere_colors[nVertices];

	for (int u = 0, ctr = 0; u < 360; u += 5) {
		for (int v = 0; v < 360; v += 5) {
			float x = radius*cos(degreeToRadians(v))*sin(degreeToRadians(u)); float y = radius*sin(degreeToRadians(v))*sin(degreeToRadians(u)); float z = radius*cos(degreeToRadians(u));
			sphere_vertices[ctr++] = position[0] + x;	sphere_vertices[ctr++] = position[1] + y;	sphere_vertices[ctr++] = position[2] + z;
		}
	}
	for (int u = 0, ctr = 0; u < 72; u += 1) {
		for (int v = 0; v < 72; v += 1) { 
			sphere_colors[ctr++] = mat->color.r; sphere_colors[ctr++] = mat->color.g; sphere_colors[ctr++] = mat->color.b;
		}
	}
	unsigned int sphere_indices[exnVertices];
	int idx = 0;
	for (int u = 0; u < 71; u += 1) {
		for (int v = 0; v < 72; v +=1) {
			sphere_indices[idx++] = v + u*72;	sphere_indices[idx++] = v + (u+1)*72;	sphere_indices[idx++] = v + (u+1)*72 + 1;
			if (v == 71)	sphere_indices[idx-1] = sphere_indices[idx-1] - v - 1;

			sphere_indices[idx++] = v + u*72;
			sphere_indices[idx++] = v + u*72 + 1;
			if (v == 71)	sphere_indices[idx-1] = sphere_indices[idx-1] - v;

			sphere_indices[idx++] = v + (u+1)*72 + 1;
			if (v == 71)	sphere_indices[idx-1] = sphere_indices[idx-1] - v - 1;
		}
	}
	for (int v = 1; v < 71; v += 1) {
		sphere_indices[idx++] = 0;
		sphere_indices[idx++] = v;
		sphere_indices[idx++] = v+1;  
	}

	for (int v = 1; v < 71; v+=1) {
		sphere_indices[idx++] = 72*72-1;
		sphere_indices[idx++] = 72*72-1-v;
		sphere_indices[idx++] = 72*72-1-v-1;
	}

	expandedVertices.clear();
	expandedColors.clear();
	expandedNormals.clear();
	for (int i = 0; i < exnVertices; i++) {
		expandedVertices.push_back(sphere_vertices[sphere_indices[i]*3]);	expandedVertices.push_back(sphere_vertices[sphere_indices[i]*3+1]);	expandedVertices.push_back(sphere_vertices[sphere_indices[i]*3+2]);
		expandedColors.push_back(sphere_colors[sphere_indices[i]*3]);	expandedColors.push_back(sphere_colors[sphere_indices[i]*3+1]);	expandedColors.push_back(sphere_colors[sphere_indices[i]*3+2]);
		expandedNormals.push_back(sphere_vertices[sphere_indices[i]*3] - position[0]);	expandedNormals.push_back(sphere_vertices[sphere_indices[i]*3+1] - position[1]);	expandedNormals.push_back(sphere_vertices[sphere_indices[i]*3+2] - position[2]);
	}
}

bool Sphere::intersect(Ray& r) const
{
	Vector3D centerVector = r.getOrigin() - position;
	double a = 1.0;
	double b = 2*dotProduct(r.getDirection(), centerVector);
	double c = dotProduct(centerVector, centerVector) - radius*radius;
	double discriminant = b*b - 4.0*a*c;

	//now check if discriminant is positive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{
		if(discriminant == 0)
		{
			double t = -b/(2.0*a);
			Vector3D p = r.getOrigin() + t*r.getDirection();
			r.setParameter(t, this, p - position);
			return true;
		}
		else
		{
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			Vector3D p = r.getOrigin() + t1*r.getDirection();
			bool b1 = r.setParameter(t1, this, p - position);
			p = r.getOrigin() + t2*r.getDirection();
			bool b2 = r.setParameter(t2, this, p - position);
			return b1||b2;
		}
	}
	return false;
}


