#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

class Camera
{
public:
  Camera();
  void rayThrough( double x, double y, ray &r, Vec3d o);
  void setEye( const Vec3d &eye );
  void setLook( double, double, double, double );
  void setLook( const Vec3d &viewDir, const Vec3d &upDir );
  void setM(Mat3d _m) {m = _m;}
  void setU(Vec3d _u) {u = _u;}
  void setV(Vec3d _v) {v = _v;}
  void setFOV(double);
  void setNH(double n) {normalizedHeight = n;}
  void setAspectRatio( double );

  const double& getAspectRatio() { return aspectRatio; }
	const Vec3d& getEye() const			{ return eye; }
	const Vec3d& getLook() const		{ return look; }
	const Vec3d& getU() const			{ return u; }
	const Vec3d& getV() const			{ return v; }
  const double& getNH() { return normalizedHeight; }
  const Mat3d& getM() { return m; }

  Mat3d m;                     // rotation matrix
  double normalizedHeight;    // dimensions of image place at unit dist from eye
  double aspectRatio;
  
  void update();              // using the above three values calculate look,u,v
  
  Vec3d eye;
  Vec3d look;                  // direction to look
  Vec3d u,v;                   // u and v in the 
};

#endif
