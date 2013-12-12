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
#include <FL/gl.h>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

LFTracer::LFTracer()
	: RayTracer() 
{
  vector<unsigned char*> bigbufs = vector<unsigned char*>();
  vector<Plane> farPlanes = vector<Plane>();
  vector<Plane> nearPlanes = vector<Plane>();
  vector<LIGHTFIELD_HEADER> headers = vector<LIGHTFIELD_HEADER>();
}

void LFTracer::rotateU(double ang) {
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW_MATRIX);
  Vec3d eye = scene->getCamera().eye;
  glRotated(ang, scene->getCamera().u[0], scene->getCamera().u[1], scene->getCamera().u[2]);
  glTranslated(eye[0], eye[1], eye[2]);
  double mat [16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mat);
  Mat3d rotMat = Mat3d(mat[0], mat[4], mat[8], mat[1], mat[5], mat[9], mat[2], mat[6], mat[10]);
  scene->getCamera().m = scene->getCamera().m * rotMat;
  scene->getCamera().update();
  glPopMatrix();
}

void LFTracer::rotateV(double ang) {
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW_MATRIX);
  Vec3d eye = scene->getCamera().eye;
  glTranslated(eye[0], eye[1], eye[2]);
  glRotated(ang, scene->getCamera().v[0], scene->getCamera().v[1], scene->getCamera().v[2]);
  double mat [16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mat);
  Mat3d rotMat = Mat3d(mat[0], mat[4], mat[8], mat[1], mat[5], mat[9], mat[2], mat[6], mat[10]);
  scene->getCamera().m = scene->getCamera().m * rotMat;
  scene->getCamera().update();
  glPopMatrix();
}

void LFTracer::getImage( double x, double y ) {

  ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY);
  scene->getCamera().rayThrough(x, y, r, Vec3d(0,0,0));

  double u, v, s, t;
  cout << "x: " << x << " y: " << y << endl;
  if (nearPlane.intersect(u, v, r, header.factor)) {
    if (farPlane.intersect(s, t, r, 1.0)) {    
      int n = header.num_pictures - 1;
      int u_index = (u + .5) * n;
      int v_index = (v + .5) * n;
      int picture_index = (v_index * (n + 1)) + u_index;
      int picture_offset = picture_index * (header.width * header.height * 3);
      memcpy(buffer, bigbuf + picture_offset, header.width*header.height*3);
    } else {
      cout << "No intersect far" << endl;
      memset(buffer, 0, header.width*header.height*3);
    }
  } else {
    cout << "No intersect near" << endl;
    memset(buffer, 0, header.width*header.height*3);
  }

  // memcpy contents of selected image into draw buffer
}

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
  if (traceUI->get360()) {
    for (int i = 0; i < 4; i++) {
      nearPlane = nearPlanes[i];
      farPlane = farPlanes[i];
      header = headers[i];
      if (nearPlane.intersect(u, v, r, header.factor)) {
        if (farPlane.intersect(s, t, r, 1.0)) {
          index = i;
          return sample(u / header.factor, v / header.factor, s, t);
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
  } else {
    if (nearPlane.intersect(u, v, r, header.factor)) {
      if (farPlane.intersect(s, t, r, 1.0)) {
        return sample(u / header.factor, v / header.factor, s, t);
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
}

Vec3d LFTracer::sample(double u, double v, double s, double t) {
  if (traceUI->get360()) {
    header = headers[index];
  }
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
  if (traceUI->get360()) {
    header = headers[index];
  }
  int s_index = (s + .5) * (header.width - 1);
  int t_index = (t + .5) * (header.height - 1);

  if (!traceUI->getSTInterp()) {
    return samplePicture(u_index, v_index, s_index, t_index);
  }
  double c00, c01, c10, c11;
  getCoeffs(c00, c01, c10, c11, s, t, header.width, header.height);
  return c00 * samplePicture(u_index, v_index, s_index, t_index) +
         c10 * samplePicture(u_index, v_index, s_index + 1, t_index) +
         c01 * samplePicture(u_index, v_index, s_index, t_index + 1) +
         c11 * samplePicture(u_index, v_index, s_index + 1, t_index + 1);
}

Vec3d LFTracer::samplePicture(int u_index, int v_index, int s_index, int t_index) {
  if (traceUI->get360()) {
    header = headers[index];
    bigbuf = bigbufs[index];
  }
  int n = header.num_pictures - 1;
  int picture_index = (v_index * (n + 1)) + u_index;
  int picture_offset = picture_index * (header.width * header.height * 3);
  int pixel_index = (t_index * header.width + s_index) * 3;
  double r = bigbuf[picture_offset + pixel_index + 0] / 255.0;
  double g = bigbuf[picture_offset + pixel_index + 1] / 255.0;
  double b = bigbuf[picture_offset + pixel_index + 2] / 255.0;
  return Vec3d(r, g, b);
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

void LFTracer::init(std::vector<LIGHTFIELD_HEADER> heads, std::vector<unsigned char*> bbufs) {
  LIGHTFIELD_HEADER h = heads[0];
  scene = new Scene;
  scene->getCamera().eye = h.camera_point - .3 * (h.camera_point - h.image_point);
  scene->getCamera().look = h.image_point - h.camera_point;
  scene->getCamera().aspectRatio = h.ar;
  scene->getCamera().normalizedHeight = h.nh;
  scene->getCamera().m = h.m;
  scene->getCamera().u = h.v1;
  scene->getCamera().v = h.v2;
  
  bigbufs = bbufs;
  
  for (int i = 0; i < 4; i++) {
    LIGHTFIELD_HEADER he = heads[i];
    farPlane = Plane();
    farPlane.origin = he.image_point;
    farPlane.u = he.v1;
    farPlane.v = he.v2;
    farPlane.n = (he.image_point - he.camera_point);
    farPlanes.push_back(farPlane);

    nearPlane = Plane();
    nearPlane.origin = he.camera_point;
    nearPlane.u = he.v1;
    nearPlane.v = he.v2;
    nearPlane.n = (he.image_point - he.camera_point);
    nearPlanes.push_back(nearPlane);
  }
  header = h;
  headers = heads;
  bigbufs = bbufs;
}

LFTracer::~LFTracer()
{
	delete scene;
	delete [] buffer;
}

bool Plane::intersect(double &u_coeff, double&v_coeff, const ray& r, double scale) {
  double dot = n * r.getDirection();
  if (dot == 0 || (dot != dot)) {
    return false;
  }
  double d = (origin - r.getPosition()) * n;
  double t = d / dot;
  
  Vec3d ip = r.at(t);
  Vec3d toPoint = ip - origin;
  double du = (toPoint * u) / u.length2();
  double ub = .5 * scale;
  double lb = -.5 * scale;

  if (du < lb || du > ub) {
    return false;
  }
  u_coeff = du;
  double dv = (toPoint * v) / v.length2();
  if (dv < lb || dv > ub) {
    return false;
  }
  v_coeff = dv;
  return true;        
}

