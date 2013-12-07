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

  double u, v, s, t;
  if (nearPlane.intersect(u, v, r)) {
    const ray new_r = rayLens(r, u, v);
    if (farPlane.intersect(s, t, new_r)) {
      return sample(u, v, s, t);
    }
    return Vec3d( .5, .5 ,.5);
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

const ray LFTracer::rayLens(const ray& r, double u, double v) {
  if (!traceUI->useDOF()) return r;
  // find closest u
  int c_u = int(u);
  if (u - c_u > .5) {
    c_u++;
  }
  int c_v = int(v);
  if (v - c_v > .5) {
    c_v++;
  }

   

  Vec3d vect = (u - c_u) * scene->getCamera().u + (v - c_v) * scene->getCamera().v;
  Vec3d new_dir = r.getDirection() + (vect * (1 / traceUI->getFocal()));
  return ray(r.getPosition(), new_dir, ray::VISIBILITY); 
}

Vec3d LFTracer::sample(double u, double v, double s, double t) {
  int n = header.num_pictures - 1;
  int u_index = (u + .5) * n;
  int v_index = (v + .5) * n;
  int s_index = (s + .5) * header.width;
  int t_index = (t + .5) * header.height;
        
  if (!traceUI->getUVInterp()) {
    return samplePicture(u_index, v_index, s_index, t_index);
  }
        
  //TODO: think about this more
  if (u == .5 || v == .5) {
    return Vec3d(0, 0, 0);
  }
    
  // determine which picture to sample
  double c00, c01, c10, c11;
  getCoeffs(c00, c01, c10, c11, u, v, n, n);
  return c00 * samplePicture(u_index, v_index, s, t) +
         c01 * samplePicture(u_index, v_index + 1, s, t) +
         c10 * samplePicture(u_index + 1, v_index, s, t) +
         c11 * samplePicture(u_index + 1, v_index + 1, s, t);
}

void LFTracer::getCoeffs(double &c00, double &c01, double &c10, double &c11, double u, double v, int u_num, int v_num) {
  int u_index = (u + .5) * u_num;
  int v_index = (v + .5) * v_num;

  double u00 = 1 - ((u + .5) * u_num - u_index);
  double v00 = 1 - ((v + .5) * v_num - v_index);
  double u10 = ((u + .5) * u_num - u_index);
  double v10 = 1 - ((v + .5) * v_num - v_index);
  double u01 = 1 - ((u + .5) * u_num - u_index);
  double v01 = ((v + .5) * v_num - v_index);
  double u11 = ((u + .5) * u_num - u_index);
  double v11 = ((v + .5) * v_num - v_index);
  
  c00 = u00 * v00;
  c01 = u01 * v01;
  c10 = u10 * v10;
  c11 = u11 * v11;
}

Vec3d LFTracer::samplePicture(int u_index, int v_index, double s, double t) {
  int s_index = (s + .5) * (header.width - 1);
  int t_index = (t + .5) * (header.height - 1);

  if (!traceUI->getSTInterp()) {
    return samplePicture(u_index, v_index, s_index, t_index);
  }

  double c00, c01, c10, c11;
  getCoeffs(c00, c01, c10, c11, s, t, header.width, header.height);
  return c11 * samplePicture(u_index, v_index, s_index, t_index) +
         c01 * samplePicture(u_index, v_index, s_index + 1, t_index) +
         c10 * samplePicture(u_index, v_index, s_index, t_index + 1) +
         c00 * samplePicture(u_index, v_index, s_index + 1, t_index + 1);
}

Vec3d LFTracer::samplePicture(int u_index, int v_index, int s_index, int t_index) {
  int n = header.num_pictures - 1;
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

void LFTracer::moveU(double d) {
  scene->getCamera().eye += d * scene->getCamera().u;
}

void LFTracer::moveV(double d) {
  scene->getCamera().eye += d * scene->getCamera().v;
}

void LFTracer::moveLook(double d) {
  scene->getCamera().eye += d * scene->getCamera().look;
}

void LFTracer::init(LIGHTFIELD_HEADER h, unsigned char* bbuf) {
  scene = new Scene;
  scene->getCamera().eye = h.camera_point - .3 * (h.camera_point - h.image_point);
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

