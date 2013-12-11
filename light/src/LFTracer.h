#ifndef __LIGHTFIELD_H__
#define __LIGHTFIELD_H__

// The main lightfield tracer.

#include "scene/ray.h"
#include "fileio/bitmap.h"
#include "RayTracer.h"

class Scene;

class Plane {
  public:
    Vec3d origin;
    Vec3d u;
    Vec3d v;
    Vec3d n;
          
    bool intersect(double &u, double &v, const ray& r, double scale);
};

class LFTracer : public RayTracer{
public:
    LFTracer();
    ~LFTracer();

  void rotateU(double ang);
  void rotateV(double ang);
  void moveU(double d);
  void moveV(double d);
  void moveLook(double d);
  void getImage( double x, double y );
  Vec3d trace( double x, double y );
  Vec3d traceRay( const ray& r, const Vec3d& thresh, int depth );
  Vec3d sample(double u, double v, double s, double t);
  void init(LIGHTFIELD_HEADER h, unsigned char* bigbuf);
  void getCoeffs(double &c00, double &c01, double &c10, double &c11, double u, double v, int u_n, int v_n);
  Vec3d samplePicture(int u_index, int v_index, double s, double t);
  Vec3d samplePicture(int u_index, int v_index, int s_index, int t_index);
  const ray rayLens(const ray& r, double u, double v);

private:
  unsigned char* bigbuf;
  Plane farPlane;
  Plane nearPlane;
  LIGHTFIELD_HEADER header;
};

#endif 
