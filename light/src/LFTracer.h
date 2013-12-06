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
          
    bool intersect(double &u, double &v);
};

class LFTracer : public RayTracer{
public:
    LFTracer();
    ~LFTracer();

  Vec3d trace( double x, double y );
  Vec3d traceRay( const ray& r, const Vec3d& thresh, int depth );
  void init(LIGHTFIELD_HEADER h, unsigned char* bigbuf);

private:
  unsigned char* bigbuf;
  Plane farPlane;
  Plane nearPlane;
};

#endif 
