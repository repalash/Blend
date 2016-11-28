//quadric.cpp

#include "quadric.h"

// bool Quadric::intersect(Ray &r) const {
// 	Vector3D d = r.getDirection(), o = r.getOrigin();
// 	double a = A*d[0]*d[0] + B*d[1]*d[1] + C*d[2]*d[2] + D*d[0]*d[1] + E*d[0]*d[1] + F*d[1]*d[2];
// 	double b = 2*A*d[0]*o[0] + 2*B*d[1]*o[1] + 2*C*d[2]*o[2] + D*(o[0]*d[1] + o[1]*d[0]) + E*(o[0]*d[2] + o[2]*d[0]) + F*(o[1]*d[2] + d[1]*o[2]) + G*d[0] + H*d[1] + I*d[2];
// 	double c = A*o[0]*o[0] + B*o[1]*o[1] + C*o[2]*o[2] + D*o[0]*o[1] + E*o[0]*o[2] + F*o[1]*o[2] + G*o[0] + H*o[1] + I*o[2] + J;
// 	double discriminant = b*b - 4.0*a*c;

// 	//now check if discriminant is positive or zero, then only we have an intersection!
// 	if(discriminant >=0.0)
// 	{
// 		if(discriminant == 0)
// 		{
// 			double t;
// 			t = -b/(2.0*a);
// 			Vector3D p = r.getOrigin() + t*r.getDirection();
// 			r.setParameter(t, this, Vector3D(2*A*p[0] + D*p[1] + E*p[2] + G, 2*B*p[1] + D*p[0] + F*p[2] + H, 2*C*p[2] + E*p[0] + F*p[1] + I));
// 			return true;
// 		}
// 		else
// 		{
// 			//Calculate both roots
// 			double D = sqrt(discriminant);
// 			double t1 = (-b +D)/(2.0*a);
// 			double t2 = (-b -D)/(2.0*a);

// 			Vector3D p = r.getOrigin() + t1*r.getDirection();
// 			bool b1 = r.setParameter(t1, this, Vector3D(2*A*p[0], 2*B*p[1], 2*C*p[2]));
// 			p = r.getOrigin() + t2*r.getDirection();
// 			bool b2 = r.setParameter(t2, this, Vector3D(2*A*p[0], 2*B*p[1], 2*C*p[2]));
// 			return b1||b2;
// 		}
// 	}
// 	return false;
// }

void Quadric::drawShape(Material* mat) {
	cout << "DRAW CYLINDER" << endl;
	const int nVertices = 3*2*180, exnVertices = 1*180*2*3 + 3*178 + 3*178;
	GLfloat cyl_vertices[nVertices];
	GLfloat cyl_colors[nVertices];

	for (int u = 0, ctr = 0; u < 10; u+=9) {
		for (int v = 0; v < 360; v += 2) {
			float x = radius*cos(degreeToRadians(v)); float z = radius*sin(degreeToRadians(v)); float y = -5+u;
			cyl_vertices[ctr++] = position[0] + x;	
			cyl_vertices[ctr++] = position[1] + y;
			cyl_vertices[ctr++] = position[2] + z;
		}
	}
	for (int u = 0, ctr = 0; u < 2; u += 1) {
		for (int v = 0; v < 180; v += 1) { 
			cyl_colors[ctr++] = mat->color.r; cyl_colors[ctr++] = mat->color.g; cyl_colors[ctr++] = mat->color.b;
		}
	}

	GLushort cyl_indices[exnVertices];
	int idx = 0;
	for (int u = 0; u < 1; u += 1) {
		for (int v = 0; v < 180; v +=1) {
			cyl_indices[idx++] = v + u*180;	cyl_indices[idx++] = v + (u+1)*180;	cyl_indices[idx++] = v + (u+1)*180 + 1;
			if (v == 179)	cyl_indices[idx-1] = cyl_indices[idx-1] - v - 1;

			cyl_indices[idx++] = v + u*180;
			cyl_indices[idx++] = v + u*180 + 1;
			if (v == 179)	cyl_indices[idx-1] = cyl_indices[idx-1] - v;

			cyl_indices[idx++] = v + (u+1)*180 + 1;
			if (v == 179)	cyl_indices[idx-1] = cyl_indices[idx-1] - v - 1;
		}
	}

	for (int v = 1; v < 179; v++) {
		cyl_indices[idx++] = 0;
		cyl_indices[idx++] = v;
		cyl_indices[idx++] = v+1;
	}
	for (int v = 1; v < 179; v++) {
		cyl_indices[idx++] = 2*180-1;
		cyl_indices[idx++] = 2*180-1-v;
		cyl_indices[idx++] = 2*180-1-v-1;
	}

	expandedVertices.clear();
	expandedColors.clear();
	expandedNormals.clear();
	for (int i = 0; i < exnVertices - 3*178 - 3*178; i++) {
		expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+1]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+2]);
		expandedColors.push_back(cyl_colors[cyl_indices[i]*3]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+1]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+2]);
		expandedNormals.push_back(cyl_vertices[cyl_indices[i]*3] - position[0]);	expandedNormals.push_back(0);	expandedNormals.push_back(cyl_vertices[cyl_indices[i]*3+2] - position[2]);
	}
	for (int i = exnVertices - 3*178 - 3*178; i < exnVertices - 3*178; i++) {
		expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+1]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+2]);
		expandedColors.push_back(cyl_colors[cyl_indices[i]*3]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+1]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+2]);
		expandedNormals.push_back(0); expandedNormals.push_back(-5); expandedNormals.push_back(0);
	}
	for (int i = exnVertices - 3*178; i < exnVertices; i++) {
		expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+1]);	expandedVertices.push_back(cyl_vertices[cyl_indices[i]*3+2]);
		expandedColors.push_back(cyl_colors[cyl_indices[i]*3]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+1]);	expandedColors.push_back(cyl_colors[cyl_indices[i]*3+2]);
		expandedNormals.push_back(0); expandedNormals.push_back(5); expandedNormals.push_back(0);
	}
}

bool Quadric::intersect(Ray &r) const {
	Vector3D d = r.getDirection(), o = r.getOrigin();

	if(B) {
		Vector3D N = Vector3D(0, 1, 0);
		double c = -3;
		double cosT = dotProduct(d, N);
		if (cosT == 0)
			return false;
		else {
			double t = -(dotProduct(o, N) + c) / cosT;
			Vector3D p = r.getOrigin() + t * r.getDirection();
//		double tanX = atan(cos(p.X())*sin(p.Y())), tanY = sin(p.X())*cos(p.Y());
			r.setParameter(t, this, Vector3D(cos(p.X() + 1), sin(p.X()) * sin(p.Z()), cos(p.Z() - 2)));
			return true;
		}
	}

	double a = A*d[0]*d[0] + B*d[1]*d[1] + C*d[2]*d[2] + D*d[0]*d[1] + E*d[0]*d[1] + F*d[1]*d[2];
	double b = 2*A*d[0]*o[0] + 2*B*d[1]*o[1] + 2*C*d[2]*o[2] + D*(o[0]*d[1] + o[1]*d[0]) + E*(o[0]*d[2] + o[2]*d[0]) + F*(o[1]*d[2] + d[1]*o[2]) + G*d[0] + H*d[1] + I*d[2];
	double c = A*o[0]*o[0] + B*o[1]*o[1] + C*o[2]*o[2] + D*o[0]*o[1] + E*o[0]*o[2] + F*o[1]*o[2] + G*o[0] + H*o[1] + I*o[2] + J;
	double discriminant = b*b - 4.0*a*c;

	//now check if discriminant is positive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{
		if(discriminant == 0)
		{
			double t;
			t = -b/(2.0*a);
			Vector3D p = r.getOrigin() + t*r.getDirection();
			if(p.Y()<3)r.setParameter(t, this, (out?1:-1)*Vector3D(2*A*p[0] + D*p[1] + E*p[2] + G, 2*B*p[1] + D*p[0] + F*p[2] + H, 2*C*p[2] + E*p[0] + F*p[1] + I));
			return true;
		}
		else
		{
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			Vector3D p = r.getOrigin() + t1*r.getDirection();
			bool b1 = p.Y()<3&&r.setParameter(t1, this, (out?1:-1)*Vector3D(2*A*p[0], 2*B*p[1], 2*C*p[2]));
			p = r.getOrigin() + t2*r.getDirection();
			bool b2 = p.Y()<3&&r.setParameter(t2, this, (out?1:-1)*Vector3D(2*A*p[0], 2*B*p[1], 2*C*p[2]));
			return b1||b2;
		}
	}
	return false;
}