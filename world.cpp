#include <iostream>
#include "world.h"
#include "sphere.h"
#include "quadric.h"
#include "Triangle.h"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

float World::firstIntersection(Ray& ray)
{
	for(int i=0; i<objectList.size(); i++)
		objectList[i]->intersect(ray);
	return ray.getParameter();
}

int World::findIntersection(Ray &r1) {
	const int obSz = (int)objectList.size();
	vector <pair < pair < float, float >, pair < float, int > > > sortObjs;
	for (int i = 0; i<(int)objectList.size(); i++) {
		float xCord = objectList[i]->getPosition()[0];
		float yCord = objectList[i]->getPosition()[1];
		float zCord = objectList[i]->getPosition()[2];
		sortObjs.push_back(make_pair(make_pair(zCord, xCord), make_pair(yCord, i)));
	}
	sort(sortObjs.begin(), sortObjs.end());
	reverse(sortObjs.begin(), sortObjs.end());

	int objPos = -1;
	for(int i1=0; i1<objectList.size(); i1++) {
		int i = sortObjs[i1].second.second;
		cout << "This : " << objectList[i]->getPosition()[2] << endl;
		if (!objectList[i]->getIsMovement()) continue;
		objectList[i]->intersect(r1);
		if(r1.didHit()) {
			return i;
		}
	}
	return objPos;
}

void World::translateObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF, int axisPos) {
	Material *m = objectList[pos]->getMaterial();
	Vector3D d1 = oldF-oldC;
	d1.normalize();
	Vector3D d2 = newF-newC;
	d2.normalize();
	Vector3D pos1 = oldC + t0*d1;
	Vector3D pos2 = newC + t0*d2;
	if (axisPos == 0) {
		pos1[1] = pos2[1] = 0;
		pos1[2] = pos2[2] = 0;
	}
	else if (axisPos == 1) {
		pos1[0] = pos2[0] = 0;
		pos1[2] = pos2[2] = 0;
	}
	else {
		pos1[2] = pos1[0];
		pos2[2] = pos2[0];
		pos1[0] = pos2[0] = 0;
		pos1[1] = pos2[1] = 0;
	}
	if ((int)objectList[pos]->getExtendedVertices().size() == 93276) {
		(dynamic_cast<Sphere *>(objectList[pos]))->changePosition(m, pos2-pos1);
	}
	else if ((int)objectList[pos]->getExtendedVertices().size() == 9) {
		(dynamic_cast<Triangle *>(objectList[pos]))->changePosition(m, pos2-pos1);
	}
	else if ((int)objectList[pos]->getExtendedVertices().size() == 6444)
		(dynamic_cast<Quadric *>(objectList[pos]))->changePosition(m, pos2-pos1); 
	else
		return ;
	cout << "TRANSLATING FOR : " << pos << " " << objectList[pos]->getPosition()[1] << " " << objectList[pos]->getPosition()[2] << endl;
	cout << "Translating Object" << endl;
}

void World::scaleObject(int pos, float t0, Vector3D oldC, Vector3D newC, Vector3D oldF, Vector3D newF) {
	Material *m = objectList[pos]->getMaterial();
	Vector3D d1 = oldF-oldC;
	d1.normalize();
	Vector3D d2 = newF-newC;
	d2.normalize();
	Vector3D pos1 = oldC + t0*d1;
	Vector3D pos2 = newC + t0*d2;
	float dis = (pos2[0]-pos1[0])*(pos2[0]-pos1[0]) + (pos2[1]-pos1[1])*(pos2[1]-pos1[1]); 
	dis = sqrt(dis);
	int side = 1;
	if (newC[0] < oldC[0])
		side = -1;
	else if (newC[0] == oldC[0] && newC[1] < oldC[1])
		side = -1;
	if ((int)objectList[pos]->getExtendedVertices().size() == 93276) {
		(dynamic_cast<Sphere *>(objectList[pos]))->changeRadius(m, side*dis);
	}
	else if ((int)objectList[pos]->getExtendedVertices().size() == 9) {
		(dynamic_cast<Triangle *>(objectList[pos]))->changeRadius(m, side*dis);
	}
	else if ((int)objectList[pos]->getExtendedVertices().size() == 6444)
		(dynamic_cast<Quadric *>(objectList[pos]))->changeRadius(m, side*dis);
	else
		return ;
	
	// cout << "SCALING FOR : " << pos << " " << objectList[pos]->getPosition()[1] << " " << objectList[pos]->getPosition()[2] << endl;
	cout << "Scaling Object" << endl;
}

Color World::shade_ray(Ray& ray)
{
	if(ray.getLevel() > MAX_LEVEL) return Color(ambient);
	firstIntersection(ray);
	if(ray.didHit()) {
		Color c = (ray.intersected())->shade(ray);
			return c;
	}
	return background;
}

void World::addLight(LightSource* ls) {
	Material *m = new Material(this);
	m->ka = 1;
	m->color = Color(ls->getIntensity());
	Sphere *sphere = new Sphere(ls->getPosition(), 8, m);
	sphere->setLightSource(ls);
	lightSourceList.push_back(ls);
	addObject(sphere);
}

void drawObject(Object * obj, GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib) {
	std::vector < GLfloat > expandedVertices = obj->getExtendedVertices();
	std::vector < GLfloat > expandedColors = obj->getExtendedColors();
	std::vector < GLfloat > expandedNormals = obj->getExtendedNormals();

	int exnVertices = expandedVertices.size();
	GLfloat *expanded_vertices = new GLfloat[exnVertices];
	GLfloat *expanded_colors = new GLfloat[exnVertices];
	glm::vec3 *expanded_normals = new glm::vec3[exnVertices/3];

	for (int i = 0; i < exnVertices; i++) { expanded_vertices[i] = expandedVertices[i]; expanded_colors[i] = expandedColors[i]; }
	for (int i = 0; i < exnVertices/3; i++) expanded_normals[i] = glm::vec3(expandedNormals[i*3], expandedNormals[i*3+1], expandedNormals[i*3+2]);

	GLuint vertex_VBO;
	glGenBuffers(1, &vertex_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, exnVertices*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete []expanded_vertices;

	GLuint color_VBO;
	glGenBuffers(1, &color_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
	glBufferData(GL_ARRAY_BUFFER, exnVertices*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor_attrib);
	glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete []expanded_colors;

	GLuint normal_VBO;
	glGenBuffers(1, &normal_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
	glBufferData(GL_ARRAY_BUFFER, (exnVertices/3)*sizeof(glm::vec3), expanded_normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormal_attrib);
	glVertexAttribPointer(vNormal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete[] expanded_normals;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void World::drawObjects(GLint vVertex_attrib, GLint vColor_attrib, GLint vNormal_attrib) {
	const int obSz = (int)objectList.size();
	vector <pair < pair < float, float >, pair < float, int > > > sortObjs;
	for (int i = 0; i<(int)objectList.size(); i++) {
		float xCord = objectList[i]->getPosition()[0];
		float yCord = objectList[i]->getPosition()[1];
		float zCord = objectList[i]->getPosition()[2];
		sortObjs.push_back(make_pair(make_pair(zCord, xCord), make_pair(yCord, i)));
	}
	sort(sortObjs.begin(), sortObjs.end());
	reverse(sortObjs.begin(), sortObjs.end());
	for(int i1=(int)objectList.size()-1; i1>=0; i1--) {
		int i = sortObjs[i1].second.second;
		if (objectList[i]->getExtendedVertices().size() > 0) {
			drawObject(objectList[i], vVertex_attrib, vColor_attrib, vNormal_attrib);
			cout << objectList[i]->getExtendedVertices().size() << " " << objectList[i]->getPosition()[2] << endl;
		    glDrawArrays(GL_TRIANGLES, 0, objectList[i]->getExtendedVertices().size());
		}
	}
}

void World::changeColor(int pos, Color nc) 	{
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


