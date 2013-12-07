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
          
    bool intersect(double &u, double &v, const ray& r);
};

class LFTracer : public RayTracer{
public:
    LFTracer();
    ~LFTracer();

  Vec3d trace( double x, double y );
  Vec3d traceRay( const ray& r, const Vec3d& thresh, int depth );
  Vec3d sample(double u, double v, double s, double t);
  void init(LIGHTFIELD_HEADER h, unsigned char* bigbuf);

private:
  unsigned char* bigbuf;
  Plane farPlane;
  Plane nearPlane;
  LIGHTFIELD_HEADER header;
};

#endif 
