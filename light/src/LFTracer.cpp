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
    double u, v, s, t;
    if (nearPlane.intersect(u, v, r) && farPlane.intersect(s, t, r)) {
      return sample(u, v, s, t);
    }
    return Vec3d( 1, 1, 1);
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

Vec3d LFTracer::sample(double u, double v, double s, double t) {
  // determine which picture to sample
  int n = header.num_pictures - 1;
  int u_index = (u + .5) * n;
  int v_index = (v + .5) * n;
  int s_index = (s + .5) * (header.width - 1);
  int t_index = (t + .5) * (header.height - 1);

  int picture_index = (v_index * (n + 1)) + u_index;
  int picture_offset = picture_index * (header.width * header.height * 3);
  int pixel_index = (t_index * header.width + s_index) * 3;
  double r = bigbuf[picture_offset + pixel_index + 0] / 255.0;
  double g = bigbuf[picture_offset + pixel_index + 1] / 255.0;
  double b = bigbuf[picture_offset + pixel_index + 2] / 255.0;
  return Vec3d(r, g, b);
}

LFTracer::LFTracer()
	: RayTracer() 
{

}

void LFTracer::init(LIGHTFIELD_HEADER h, unsigned char* bbuf) {
  //TODO: Build scene
  scene = new Scene;
  scene->getCamera().eye = h.camera_point + .3 * (h.camera_point - h.image_point);
  scene->getCamera().look = h.image_point - h.camera_point;
  scene->getCamera().aspectRatio = h.ar;
  scene->getCamera().normalizedHeight = h.nh;
  scene->getCamera().m = h.m;
  scene->getCamera().u = h.v1;
  scene->getCamera().v = h.v2;
  
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

  header = h;
}

LFTracer::~LFTracer()
{
	delete scene;
	delete [] buffer;
}

bool Plane::intersect(double &u_coeff, double&v_coeff, const ray& r) {
  double d = n * origin;
  d -= n * r.getPosition();
  double dot = n * r.getDirection();
  if (dot == 0 || (dot != dot)) {
    return false;
  }
  double t = d / dot;
  if (t < RAY_EPSILON) {
    return false;
  }
  Vec3d ip = r.at(t);
  Vec3d toPoint = ip - origin;
  double du = (toPoint * u) / u.length2();
  if (du < -.5 || du > .5) {
    return false;
  }
  u_coeff = du;
  double dv = (toPoint * v) / v.length2();
  if (dv < -.5 || dv > .5) {
    return false;
  }
  v_coeff = dv;
  return true;        
}

