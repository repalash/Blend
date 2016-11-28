#include <iostream>
#include "world.h"
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

float World::firstIntersection(Ray& ray)
{
	for(int i=0; i<objectList.size(); i++)
		objectList[i]->intersect(ray);
	return ray.getParameter();
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

void World::addLight(Object* ls) {
	if(!ls->isLightSource()) return;
	lightSourceList.push_back((LightSource *const &) ls->getLightSource());
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