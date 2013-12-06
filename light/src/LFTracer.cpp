// The main ray tracer.

#pragma warning (disable: 4786)

#include "LFTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

Vec3d LFTracer::trace( double x, double y ) {
	// Clear out the ray cache in the scene for debugging purposes,
	scene->intersectCache.clear();

  ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY );

  scene->getCamera().rayThrough( x,y,r,Vec3d(0,0,0));

  double cutoff = traceUI->getCutoff();
	Vec3d ret = traceRay( r, Vec3d(cutoff,cutoff,cutoff), 0 );
	ret.clamp();
	return ret;
}

Vec3d LFTracer::traceRay( const ray& r, const Vec3d& thresh, int depth )
{
	isect i;

  // TODO: Manually intersect both planes
	if( true ) {
    return Vec3d(1.0, 0.0, 0.0);

  } else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
    if (scene->cubeMap()) {
      // TODO: Extract cubeMap code to other method and call that here 
		  return Vec3d( 0.0, 0.0, 0.0 );
    } else {
		  return Vec3d( 0.0, 0.0, 0.0 );
	  }
  }
}

LFTracer::LFTracer()
	: RayTracer() 
{

}

void LFTracer::init(LIGHTFIELD_HEADER h, unsigned char* bbuf) {
  //TODO: Build scene
  scene = new Scene;
  scene->getCamera().setEye(h.camera_point + (h.camera_point - h.image_point));
  scene->getCamera().setFOV(h.fov);
  scene->getCamera().setAspectRatio(h.ar);
  scene->getCamera().setLook(h.image_point - h.camera_point, h.v2);
  bigbuf = bbuf;
  
  farPlane = Plane();
  farPlane.origin = h.image_point;
  farPlane.u = h.v1;
  farPlane.v = h.v2;
  farPlane.n = (h.image_point - h.camera_point);

  nearPlane = Plane();
  nearPlane.origin = h.camera_point;
  nearPlane.u = h.v1;
  nearPlane.v = h.v2;
  nearPlane.n = (h.image_point - h.camera_point);
}

LFTracer::~LFTracer()
{
	delete scene;
	delete [] buffer;
}

bool Plane::intersect(double &u, double&v) {
  //TODO: this
  return false; 
}

