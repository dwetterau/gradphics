#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/ray.h"
#include "fileio/bitmap.h"

class Scene;

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

  virtual Vec3d trace( double x, double y );
	virtual Vec3d traceRay( const ray& r, const Vec3d& thresh, int depth );

  
  void rotateV(double ang);
  LIGHTFIELD_HEADER getLightfieldHeader();
	void getBuffer( unsigned char *&buf, int &w, int &h );
	void copyBufferTo( unsigned char *&buf, int size );
	double aspectRatio();
	void traceSetup( int w, int h );
	void tracePixel( int i, int j );
  void reloadScene();
  bool loadScene( char* fn );
    
	bool sceneLoaded() { return scene != 0; }
  void setReady( bool ready )
      { m_bBufferReady = ready; }
  bool isReady() const
      { return m_bBufferReady; }

	const Scene& getScene() { return *scene; }
  void setEyePos(double u, double v);
  void moveEyeByLook(double amt);
  double cur_u;
  double cur_v;
protected:
	Vec3d eye_origin;
  unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene* scene;

  bool m_bBufferReady;


};

#endif // __RAYTRACER_H__
