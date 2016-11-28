/*References
  Trackball: http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glui.h>
#include <GL/gl.h>
#endif

#include <IL/il.h> //TODO
#include <IL/ilu.h>

#include "shader_utils.h"
#include "gl_utils.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "lightsource.h"
#include "pointlightsource.h"
#include "triangle.h"
#include "quadric.h"
#include "cylinder.h"

#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

//Globals
GLuint program;
GLint attribute_coord2d;
int screen_width = 900, screen_height = 600; //Both even numbers
float quadVertices[] = {-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1}; //2D screen space coordinates
GLuint texImage;
GLint uniform_texImage;

Camera *camera;
RenderEngine *engine;
World *world;

GLuint program2;
GLint vVertex_attrib, vColor_attrib, vNormal_attrib;
GLint vModel_uniform, vView_uniform, vProjection_uniform;
// GLint vKa, vKs, vKd;
GLint vLightPosition_uniform[2], vLightColor_uniform[2];

glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations

void setupLightPosition(std::vector<LightSource *>);
void setupLightColor(std::vector<LightSource *>);
void setupModelTransformation();
void setupViewTransformation(Vector3D, Vector3D, Vector3D);
void setupProjectionTransformation(float, float);

int redBoxValue = 0, greenBoxValue = 0, blueBoxValue = 0;
float krBoxVaue = 0.0, etaBoxVaue = 0.0;
int ktBoxVaue = 0, phongnBoxVaue = 0;
int nSamplesBoxValue = 1;
int axisBoxValue = 0;

int init_resources(void)
{
	program2 = createProgram("vshader_2.vs", "fshader_2.fs");
	glUseProgram(program2);

	//Bind shader variables
	vVertex_attrib = glGetAttribLocation(program2, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}
	vColor_attrib = glGetAttribLocation(program2, "vColor");
	if(vColor_attrib == -1) {
		fprintf(stderr, "Could not bind location: vColor\n");
		exit(0);
	}
	vNormal_attrib = glGetAttribLocation(program2, "vNormal");
	if(vNormal_attrib == -1) {
		fprintf(stderr, "Could not bind location: vNormal\n");
		exit(0);
	}

	// vKa = glGetUniformLocation(program2, "vKa");
	// if(vKa == -1){
	// 	fprintf(stderr, "Could not bind location: vKa\n");
	// 	exit(0);
	// }

	// vKs = glGetUniformLocation(program2, "vKs");
	// if(vKs == -1){
	// 	fprintf(stderr, "Could not bind location: vKs\n");
	// 	exit(0);
	// }

	// vKd = glGetUniformLocation(program2, "vKd");
	// if(vKd == -1){
	// 	fprintf(stderr, "Could not bind location: vKd\n");
	// 	exit(0);
	// }

	//Create program
	program = createProgram("vshader.vs", "fshader.fs");
	attribute_coord2d = glGetAttribLocation(program, "coord2d");
	if(attribute_coord2d == -1)
	{
		fprintf(stderr, "Could not bind location: coord2d\n");
		return 0;
	}

	//Initialize raytracer objects
	Vector3D camera_position(0, 3, 16);
	Vector3D camera_target(0,0,0); //Looking down -Z axis
	Vector3D camera_up(0, 1, 0);
	float camera_fovy =  45;
	camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, screen_width, screen_height);
	//Create a world

	world = new World;
	world->setAmbient(Color(1));
	world->setBackground(Color(0, 0, 0));

	Material *m = new Material(world); //diffuse red
	m->color = Color(1, 0.25, .25); m->ka = 0.; m->kd = 0; m->ks = 0.8; m->katt = 0.005; m->kr = 0; m->n = 0; m->eta = 1.5; m->kt = 0;
	Material *m2 = new Material(world); //specular
	m2->color = Color(1, 0.65, 0.85); m2->ka = 0.1; m2->kd = 1; m2->ks = 0.; m2->katt = 0.005; m2->kr = 0.999; m2->n = 256; m2->kt = 0; m2->eta = 1.4;
	Material *m3 = new Material(world); //dielectric
	m3->color = Color(1, 1, 1); m3->ka = 0.2; m3->kd = 1; m3->ks = 0.1; m3->katt = 0.005; m3->kr = 0.2; m3->n = 128; m3->kt = 1; m3->eta = 1.33;
	Material *m5 = new Material(world); //dielectric
	m5->color = Color(1, 0.95, 0.95); m5->eta = 1.75; m5->kt=1;
	Material *m4 = new Material(world); //glossy
	m4->color = Color(1, 1, 0.23);  m4->n = 20;

	Material *mPlane = new Material(world);
	mPlane->color = Color(0.25, 0.75, 0.25); mPlane->kr = 0;
	Material *mPlane2 = new Material(world);
	mPlane2->color = Color(0.25, 0.25, 0.75); mPlane2->kr = 0;
	Material *mPlane3 = new Material(world);
	mPlane3->color = Color(0.75, 0.25, 0.25); mPlane3->kr = 0;
	Material *mPlane4 = new Material(world);
	mPlane4->color = Color(0.67, 0.67, 0.67); mPlane4->kr = 0;

//	world->addObject(new Sphere(Vector3D( 1e5+1,40.8,81.6), 1e5, m));
//	world->addObject(new Sphere(Vector3D(-1e5+99,40.8,81.6), 1e5, m));
//	world->addObject(new Sphere(Vector3D(50,40.8, 1e5),     1e5, m));
//	world->addObject(new Sphere(Vector3D(50,40.8,-1e5+170), 1e5, m));
//	world->addObject(new Sphere(Vector3D(50, 1e5, 81.6),    1e5, m));
//	world->addObject(new Sphere(Vector3D(50,-1e5+81.6,81.6),1e5, m));
//	world->addObject(new Sphere(Vector3D(27,16.5,47),       16 , m));
//	world->addObject(new Sphere(Vector3D(73,16.5,78),       16 , m));
//	world->addObject(new Sphere(Vector3D(50,681.6-.27,81.6),600, m));


//	Object *sphere = new Sphere(Vector3D(3, -2, -6), 3, m2);
//	world->addObject(sphere);
////
	Object *sphere5 = new Sphere(Vector3D(0, -4, 0), 0.7, m4);
	world->addObject(sphere5);
////
//	Object *sphere2 = new Sphere(Vector3D(-3.5, -3, 3.6), 1.6, m5);
//	world->addObject(sphere2);

//	Object *sphere4 = new Sphere(Vector3D(-1, -2, -12), 3, m4);
//	world->addObject(sphere4);
//
//	Object *sphere3 = new Sphere(Vector3D(-2, -3.5, -5), 1.5, m2);
//	world->addObject(sphere3);

//	Object *sphere2 = new Sphere(Vector3D(2, 1.5, -5), 1.4, m2);
//	world->addObject(sphere2);

//	Object *quadric = new Quadric(0.1, 1, 0.1, 0, 0, 0, 0, 0, 0, -1, true, m3);  //water, sort of..
//	world->addObject(quadric);
//	Object *quadric2 = new Quadric(0.099, 0, 0.099, 0, 0, 0, 0, 0, 0, -1, true, m2);
//	Object *quadric2 = new Quadric(0.1, 0, 0.1, 0, 0, 0, 0, 0, 0, -1, m2);
//	world->addObject(quadric2);

	// Object *quadric3 = new Quadric(Vector3D(0,0,0), 0.3, 0, 0.3, 0, 0, 0, 0, 0, 0, -1, false, m5);
	// world->addObject(quadric3);
	// Object *quadric4 = new Quadric(Vector3D(0,0,0), 0.1, 0, 0.1, 0, 0, 0, 0, 0, 0, -1, true, m5);
	// world->addObject(quadric4);

//	Object *triangle = new Triangle(Vector3D(0, 10, -15), Vector3D(-8, 0, -18), Vector3D(8, 0, -18), m3);
//	world->addObject(triangle);

	Object *planeTriangle1 = new Triangle(Vector3D(-50, -5, 25), Vector3D(50, -5, 25), Vector3D(50, -5, -50), mPlane2);
	world->addObject(planeTriangle1);
	Object *planeTriangle2 = new Triangle(Vector3D(-50, -5, 25), Vector3D(50, -5, -50), Vector3D(-50, -5, -50), mPlane2);
	world->addObject(planeTriangle2);

	Object *planeTriangle3 = new Triangle(Vector3D(50, 25, -10), Vector3D(-50, 25, -10), Vector3D(50, -6, -10), mPlane4);
	world->addObject(planeTriangle3);
	Object *planeTriangle4 = new Triangle(Vector3D(50, -6, -10), Vector3D(-50, 25, -10), Vector3D(-50, -6, -10), mPlane4);
	world->addObject(planeTriangle4);

	Object *planeTriangle5 = new Triangle(Vector3D(6, -10, 20), Vector3D(6, 10, 20), Vector3D(6, 10, -20), mPlane3);
	world->addObject(planeTriangle5);
	Object *planeTriangle6 = new Triangle(Vector3D(6, -10, -20), Vector3D(6, -10, 20), Vector3D(6, 10, -20), mPlane3);
	world->addObject(planeTriangle6);
	Object *planeTriangle7 = new Triangle(Vector3D(-6, 10, 20), Vector3D(-6, -10, 20), Vector3D(-6, 10, -20), mPlane);
	world->addObject(planeTriangle7);
	Object *planeTriangle8 = new Triangle(Vector3D(-6, -10, 20), Vector3D(-6, -10, -20), Vector3D(-6, 10, -20), mPlane);
	world->addObject(planeTriangle8);

//	Object *planeTriangle9 = new Triangle(Vector3D(50, 5, 25), Vector3D(-50, 5, 25), Vector3D(50, 5, -50), mPlane4);
//	world->addObject(planeTriangle9);
//	Object *planeTriangle10 = new Triangle(Vector3D(50, 5, -50), Vector3D(-50, 5, 25), Vector3D(-50, 5, -50), mPlane4);
//	world->addObject(planeTriangle10);

	LightSource *light = new PointLightSource(world, Vector3D(0, 20, 10), Color(30,30,30));
	world->addLight(light);

//	LightSource *light2 = new PointLightSource(world, Vector3D(0, 10, 10), Color(20, 20, 20));
//	world->addLight(light2);

//	LightSource *light3 = new PointLightSource(world, Vector3D(-8, 0, 0), Color(0.2, 0, 0));
//	world->addLight(light3);

	setupLightPosition(world->getLightSourceList());
	setupLightColor(world->getLightSourceList());
	setupModelTransformation();
	setupViewTransformation(camera->get_position(), camera->get_target(), camera->get_up());
	setupProjectionTransformation(camera->get_aspect(), camera->get_fovy());

	engine = new RenderEngine(world, camera, nSamplesBoxValue);
	engine->setRenderFlag(false);

	//Initialise texture
	glGenTextures(1, &texImage);
	glBindTexture(GL_TEXTURE_2D, texImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //Show pixels when zoomed
	uniform_texImage = glGetUniformLocation(program, "texImage");
	if(uniform_texImage == -1)
	{
		fprintf(stderr, "Could not bind uniform: texImage\n");
		return 0;
	}
	return 1;
}

void onDisplay()
{
	/* Clear the background as white */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (engine->getRenderFlag()) {
		glUseProgram(program);
		glEnableVertexAttribArray(attribute_coord2d);
		glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, quadVertices);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texImage);
		glUniform1i(uniform_texImage, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(attribute_coord2d);
	}
	else {
		engine->renderObjects(vVertex_attrib, vColor_attrib, vNormal_attrib);
		glUseProgram(program2);
		glBindVertexArray(0);
	}

	/* Display the result */
	glutSwapBuffers();
}

void free_resources()
{
	glDeleteProgram(program);
	glDeleteProgram(program2);
	glDeleteTextures(1, &texImage);
}

void onReshape(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

void SaveImage()  //TODO:
{
	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilTexImage(camera->getWidth(), camera->getHeight(), 1, 3, IL_RGB, IL_UNSIGNED_BYTE, camera->getBitmap());
	//ilEnable(IL_FILE_OVERWRITE);
	time_t rawtime;
	time (&rawtime);
	char time_str[26];
	ctime_r(&rawtime, time_str);
	time_str[strlen(time_str) - 1] =0;//Remove trailing return character.
	char imageName[256];
	sprintf(imageName, "Lumina %s.png", time_str);
	ilSave(IL_PNG, imageName);
	fprintf(stderr, "Image saved as: %s\n", imageName);
}

bool scaleFlag = false, translateFlag = false, addSphereFlag = false, changeColorFlag = false, changeMaterialFlag = false;
bool isDragging = false;
int currentX, currentY, oldX, oldY;
pair < int, float > checkObjectPos = make_pair(-1, -1);
int displayCtr = 0;

void onKey(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: exit(0);
			break;
		case 's': //Save to image
		case 'S': //Save to image
			SaveImage();
			break;
		case 'r': 
			camera->resetSteps();
			engine->setStartPos(0);
			engine->setRenderFlag(true);
			break;
		case 'R': 
			camera->resetSteps();
			engine->setStartPos(0);
			engine->setRenderFlag(true);
			break;
		case 'e': engine->setRenderFlag(false);
			displayCtr++;
			cout << "Pressed button" << endl;
			break;
		case 'E': engine->setRenderFlag(false);
			displayCtr++;
			break;
		case 'Z': if (!engine->getRenderFlag()) {
					scaleFlag = true;
					translateFlag = false;
				}
			break;
		case 'z': if (!engine->getRenderFlag()) {
					scaleFlag = true;
					translateFlag = false;
				}
			break;
		case 'T': if (!engine->getRenderFlag()) {
					scaleFlag = false;
					translateFlag = true;
				}
			break;
		case 't': if (!engine->getRenderFlag()) {
					scaleFlag = false;
					translateFlag = true;
				}
			break;
	}
}




void onMouseButton(int button, int state, int x, int y)
{
	if (engine->getRenderFlag()) return ;
	cout << "Mouse Button Pressed" << endl;
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			currentX = oldX = x;
			currentY = oldY = y;
			cout << x << " " << y << endl;
			cout << redBoxValue << " " << greenBoxValue << " " << blueBoxValue << endl;
			glm::mat4 camera2world = glm::inverse(glm::mat4(projectionT*viewT*modelT));
			glm::vec4 resultCoordNearPlane = camera2world * glm::vec4(2.0*x/screen_width - 1.0, 1.0 - 2.0*y/screen_height, 0.0, 1.0);
			glm::vec4 resultCoordFarPlane = camera2world * glm::vec4(2.0*x/screen_width - 1.0, 1.0 - 2.0*y/screen_height, 1.0, 1.0);
			glm::vec3 r11(resultCoordNearPlane[0]/resultCoordNearPlane[3], resultCoordNearPlane[1]/resultCoordNearPlane[3], resultCoordNearPlane[2]/resultCoordNearPlane[3]);
			glm::vec3 r22(resultCoordFarPlane[0]/resultCoordFarPlane[3], resultCoordFarPlane[1]/resultCoordFarPlane[3], resultCoordFarPlane[2]/resultCoordFarPlane[3]);

			if (translateFlag || scaleFlag) {
				checkObjectPos = engine->checkIntersect(Vector3D(r11[0], r11[1], r11[2]), Vector3D(r22[0], r22[1], r22[2]));
				if (checkObjectPos.first != -1) {
					isDragging = true;
				}
			}
			else if (addSphereFlag) {
				Vector3D initDir = Vector3D(r22[0], r22[1], r22[2]) - Vector3D(r11[0], r11[1], r11[2]);
				initDir.normalize();
				Vector3D initPos = Vector3D(r11[0], r11[1], r11[2]) + camera->get_position()[2]*(initDir);
				cout << initPos[0] << " " << initPos[1] << " " << initPos[2] << endl;
				initPos[2] = 0;
 				Material *m = new Material(world);
				m->color = Color(redBoxValue/255.0, greenBoxValue/255.0, blueBoxValue/255.0); m->ka = 0.2; m->kd = 1; m->ks = 0.0; m->katt = 0.005; m->kr = krBoxVaue; m->n = phongnBoxVaue; m->kt = ktBoxVaue; m->eta = etaBoxVaue;
				Object *sphere = new Sphere(initPos, 2, m);
				world->addObject(sphere);
				addSphereFlag = false;
				glutPostRedisplay();
			}
			else if (changeColorFlag) {
				checkObjectPos = engine->checkIntersect(Vector3D(r11[0], r11[1], r11[2]), Vector3D(r22[0], r22[1], r22[2]));
				if (checkObjectPos.first != -1)  {
					engine->changeColor(checkObjectPos.first, Color(redBoxValue/255.0, greenBoxValue/255.0, blueBoxValue/255.0));
					glutPostRedisplay();
				}
			}
			else if (changeMaterialFlag) {
				checkObjectPos = engine->checkIntersect(Vector3D(r11[0], r11[1], r11[2]), Vector3D(r22[0], r22[1], r22[2]));
				if (checkObjectPos.first != -1)  {
					engine->changeMaterial(checkObjectPos.first, krBoxVaue, ktBoxVaue, etaBoxVaue, phongnBoxVaue);
					glutPostRedisplay();
				}
			}
		}
		else {
			isDragging = false;
		}
	}
}

void onMouseMotion(int x, int y)
{
	if (engine->getRenderFlag()) return ;
	if(isDragging)
	{
		currentX = x;
		currentY = y;
	}
}

void onIdle(void)
{
	if (displayCtr > 0) {
		displayCtr = 0;
		glutPostRedisplay();
	}

	static bool done = false;
	//Generate a pretty picture
	if (engine->getRenderFlag()) {
		if(!done)
		{
			for(int i=0; i < screen_width/10; i++)
				if(engine->renderLoop())
				{
					done = true;
					fprintf(stderr, "Rendering complete.\n");
				}

			//Update texture on GPU
			glBindTexture(GL_TEXTURE_2D, texImage);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());

			glutPostRedisplay();
		}
	}
	else {
		done = false;
		if((currentX !=oldX || currentY != oldY) && (translateFlag || scaleFlag)) {
			cout << currentX << " " << currentY << endl;
			// cout << "OBJECT NO " << checkObjectPos.first << endl;
			glm::mat4 camera2world1 = glm::inverse(glm::mat4(projectionT*viewT*modelT));
			glm::vec4 oldCoordOrigin = camera2world1 * glm::vec4(2.0*oldX/screen_width - 1.0, 1.0 - 2.0*oldY/screen_height, 0.0, 1.0);
			glm::vec4 newCoordOrigin = camera2world1 * glm::vec4(2.0*currentX/screen_width - 1.0, 1.0 - 2.0*currentY/screen_height, 0.0, 1.0);
			oldCoordOrigin[0] /= oldCoordOrigin[3];
			oldCoordOrigin[1] /= oldCoordOrigin[3];
			oldCoordOrigin[2] /= oldCoordOrigin[3];
			newCoordOrigin[0] /= newCoordOrigin[3];
			newCoordOrigin[1] /= newCoordOrigin[3];
			newCoordOrigin[2] /= newCoordOrigin[3];

			glm::vec4 oldCoordFar = camera2world1 * glm::vec4(2.0*oldX/screen_width - 1.0, 1.0 - 2.0*oldY/screen_height, 1.0, 1.0);
			oldCoordFar[0] /= oldCoordFar[3];
			oldCoordFar[1] /= oldCoordFar[3];
			oldCoordFar[2] /= oldCoordFar[3];

			glm::vec4 newCoordFar = camera2world1 * glm::vec4(2.0*currentX/screen_width - 1.0, 1.0 - 2.0*currentY/screen_height, 1.0, 1.0);
			newCoordFar[0] /= newCoordFar[3];
			newCoordFar[1] /= newCoordFar[3];
			newCoordFar[2] /= newCoordFar[3];

			if (translateFlag)
				engine->translateObject(checkObjectPos.first, checkObjectPos.second, Vector3D(oldCoordOrigin[0], oldCoordOrigin[1], oldCoordOrigin[2]), Vector3D(newCoordOrigin[0], newCoordOrigin[1], newCoordOrigin[2]), Vector3D(oldCoordFar[0], oldCoordFar[1], oldCoordFar[2]), Vector3D(newCoordFar[0], newCoordFar[1], newCoordFar[2]), axisBoxValue);
			else if (scaleFlag) {
			//	cout << "SCALE HERE : " << checkObjectPos.first << endl;
				engine->scaleObject(checkObjectPos.first, checkObjectPos.second, Vector3D(oldCoordOrigin[0], oldCoordOrigin[1], oldCoordOrigin[2]), Vector3D(newCoordOrigin[0], newCoordOrigin[1], newCoordOrigin[2]), Vector3D(oldCoordFar[0], oldCoordFar[1], oldCoordFar[2]), Vector3D(newCoordFar[0], newCoordFar[1], newCoordFar[2]));
			}
			oldX = currentX;
			oldY = currentY;
			glutPostRedisplay();
		}
	}
}

void glui_cb1(int control) {
	if (!engine->getRenderFlag()) {
		scaleFlag = false;
		addSphereFlag = false;
		translateFlag = true;
		changeColorFlag = false;
		changeMaterialFlag = false;
	}
	cout << "Translate option selected" << endl;
}

void glui_cb2(int control) {
	if (!engine->getRenderFlag()) {
		scaleFlag = true;
		addSphereFlag = false;
		translateFlag = false;
		changeColorFlag = false;
		changeMaterialFlag = false;
	}
	cout << "Scale option selected" << endl;
}

void glui_cb3(int control) {
	if (!engine->getRenderFlag()) {
		addSphereFlag = true;
		scaleFlag = false;
		translateFlag = false;
		changeColorFlag = false;
		changeMaterialFlag = false;
	}
	cout << "Sphere added" << endl;
}

void glui_cb4(int control) {
	if (!engine->getRenderFlag()) {
		addSphereFlag = false;
		scaleFlag = false;
		translateFlag = false;
		changeColorFlag = true;
		changeMaterialFlag = false;
	}
	cout << "Change color" << endl;
}

void glui_cb5(int control) {
	if (!engine->getRenderFlag()) {
		addSphereFlag = false;
		scaleFlag = false;
		translateFlag = false;
		changeColorFlag = false;
		changeMaterialFlag = true;
	}
	cout << "Change material" << endl;
}

GLUI_Spinner * redSpin, * greenSpin, * blueSpin;
GLUI_Spinner * krSpin, * ktSpin, * etaSpin, * phongnSpin, * numSamplesSpin;
GLUI_RadioGroup *group1;

void updateRedBox(int id) { redBoxValue = redSpin->get_int_val(); }
void updateGreenBox(int id) { greenBoxValue = greenSpin->get_int_val(); }
void updateBlueBox(int id) { blueBoxValue = blueSpin->get_int_val(); }
void updateKrBox(int id) { krBoxVaue = krSpin->get_float_val(); }
void updateKtBox(int id) { ktBoxVaue = ktSpin->get_int_val(); }
void updateEtaBox(int id) { etaBoxVaue = etaSpin->get_float_val(); }
void updatePhongnBox(int id) { phongnBoxVaue = phongnSpin->get_int_val(); }
void updatenSampleBox(int id) { nSamplesBoxValue = numSamplesSpin->get_int_val(); engine->setSamples(nSamplesBoxValue);};
void axisSelectBox(int id) { axisBoxValue = group1->get_int_val(); }

void widgets(int majorWindow) {
	GLUI * subwindow1 = GLUI_Master.create_glui_subwindow(majorWindow, GLUI_SUBWINDOW_RIGHT);
	subwindow1->set_main_gfx_window(majorWindow);
	GLUI_Panel * panel1 = subwindow1->add_panel("Object Panel", GLUI_PANEL_EMBOSSED);
	GLUI_Button * button1 = subwindow1->add_button_to_panel (panel1, "Translate", -1, glui_cb1);
	group1 = subwindow1->add_radiogroup_to_panel(panel1, NULL, 3, axisSelectBox);
 	subwindow1->add_radiobutton_to_group(group1, "X" );
 	subwindow1->add_radiobutton_to_group(group1, "Y" );
 	subwindow1->add_radiobutton_to_group(group1, "Z" );
	GLUI_Button * button2 = subwindow1->add_button_to_panel (panel1, "Scale", -1, glui_cb2);
	GLUI_Button * button4 = subwindow1->add_button_to_panel (panel1, "Change Color", -1, glui_cb4);
	GLUI_Button * button5 = subwindow1->add_button_to_panel (panel1, "Change Material", -1, glui_cb5);

	GLUI_Panel * panel2 = subwindow1->add_panel("Add Objects", GLUI_PANEL_EMBOSSED);
	GLUI_Button * button3 = subwindow1->add_button_to_panel (panel2, "Sphere", -1, glui_cb3);

	GLUI_Panel * panel3 = subwindow1->add_panel("Color", GLUI_PANEL_EMBOSSED);
	redSpin = subwindow1->add_spinner_to_panel(panel3, "Red", GLUI_SPINNER_INT, NULL, 0, updateRedBox); redSpin->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	greenSpin = subwindow1->add_spinner_to_panel(panel3, "Green", GLUI_SPINNER_INT, NULL, 0, updateGreenBox); greenSpin->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);
	blueSpin = subwindow1->add_spinner_to_panel(panel3, "Blue", GLUI_SPINNER_INT, NULL, 0, updateBlueBox); blueSpin->set_int_limits(0, 255, GLUI_LIMIT_CLAMP);

	GLUI_Panel * panel4 = subwindow1->add_panel("Material", GLUI_PANEL_EMBOSSED);
	krSpin = subwindow1->add_spinner_to_panel(panel4, "kr", GLUI_SPINNER_FLOAT, NULL, 0, updateKrBox); krSpin->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
	ktSpin = subwindow1->add_spinner_to_panel(panel4, "kt", GLUI_SPINNER_INT, NULL, 0, updateKtBox); ktSpin->set_int_limits(0, 1, GLUI_LIMIT_CLAMP);
	etaSpin = subwindow1->add_spinner_to_panel(panel4, "eta", GLUI_SPINNER_FLOAT, NULL, 0, updateEtaBox); etaSpin->set_float_limits(0.0, 100, GLUI_LIMIT_CLAMP);
	phongnSpin =  subwindow1->add_spinner_to_panel(panel4, "n", GLUI_SPINNER_INT, NULL, 0, updatePhongnBox); phongnSpin->set_int_limits(0, 500, GLUI_LIMIT_CLAMP);
	GLUI_Master.set_glutIdleFunc(onIdle);

	GLUI_Panel * panel5 = subwindow1->add_panel("Parameters", GLUI_PANEL_EMBOSSED);
	numSamplesSpin = subwindow1->add_spinner_to_panel(panel5, "nSamples", GLUI_SPINNER_INT, NULL, 0, updatenSampleBox); numSamplesSpin->set_int_limits(1, 30, GLUI_LIMIT_CLAMP);

}

int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		screen_width = atoi(argv[1]);
		screen_height = atoi(argv[2]);
		screen_width -= (screen_width % 2); //Make it even
		screen_height -= (screen_height % 2); //Make it even
	}
	fprintf(stderr, "Welcome to Blend pathtracer and editor.\nFull command: %s [width] [height]\nPress 's' to save framebufer to disk.\n", argv[0]);
	/* Glut-related initialising functions */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(screen_width, screen_height);
	int majorWindow =  glutCreateWindow("Blend: Pathtracer and editor");
#ifndef __APPLE__
	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
#endif

	ilInit(); //TODO

	/* When all init functions run without errors,
	   the program can initialise the resources */
	if (1 == init_resources())
	{
		/* We can display it if everything goes OK */
		glutReshapeFunc(onReshape);
		glutDisplayFunc(onDisplay);
		glutMouseFunc(onMouseButton);
		glutMotionFunc(onMouseMotion);
		glutKeyboardFunc(onKey);
		glutIdleFunc(onIdle);
		widgets(majorWindow);
		glutMainLoop();
	}

	/* If the program exits in the usual way,
	   free resources and exit with a success */
	free_resources();
	return EXIT_SUCCESS;
}

void setupLightPosition(std::vector<LightSource *> lightSourceList) {
	glUseProgram(program2);
	for (int i = 0; i < (int)lightSourceList.size(); i++) {
		if (i == 0)
			vLightPosition_uniform[i] = glGetUniformLocation(program2, "vLightPosition[0]");
		else
			vLightPosition_uniform[i] = glGetUniformLocation(program2, "vLightPosition[1]");
		if(vLightPosition_uniform[i] == -1){
			fprintf(stderr, "Could not bind location: vLightPosition\n");
			exit(0);
		}
		glUniform3f(vLightPosition_uniform[i], lightSourceList[i]->getPosition()[0], lightSourceList[i]->getPosition()[1], lightSourceList[i]->getPosition()[2]);
	}
}

void setupLightColor(std::vector<LightSource *> lightSourceList) {
	glUseProgram(program2);
	for (int i = 0; i < (int)lightSourceList.size(); i++) {
		if (i == 0)
			vLightColor_uniform[i] = glGetUniformLocation(program2, "vLightColor[0]");
		else
			vLightColor_uniform[i] = glGetUniformLocation(program2, "vLightColor[1]");
		if(vLightColor_uniform[i] == -1){
			fprintf(stderr, "Could not bind location: vLightColor\n");
			exit(0);
		}
		glUniform3f(vLightColor_uniform[i], lightSourceList[i]->getIntensity().r, lightSourceList[i]->getIntensity().g, lightSourceList[i]->getIntensity().b);
	}
}

void setupModelTransformation()
{
	//Modelling transformations (Model -> World coordinates)
	modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

	glUseProgram(program2);
	vModel_uniform = glGetUniformLocation(program2, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupViewTransformation(Vector3D position, Vector3D target, Vector3D up)
{
	//Viewing transformations (World -> Camera coordinates
	viewT = glm::lookAt(glm::vec3(position[0], position[1], position[2]), glm::vec3(target[0], target[1], target[2]), glm::vec3(up[0], up[1], up[2]));

	glUseProgram(program2);
	vView_uniform = glGetUniformLocation(program2, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(float aspect, float fovy)
{
	float view_height = 50.0f;
	projectionT = glm::perspective(fovy, aspect, 0.1f, 1000.0f);

	glUseProgram(program2);
	vProjection_uniform = glGetUniformLocation(program2, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}