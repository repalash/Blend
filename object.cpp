#include "object.h"
#include "pointlightsource.h"

void Object::setLightSource(Color color, World *world) 	{
	lightSource = new PointLightSource(world, Vector3D(0,0,0), color);
}
