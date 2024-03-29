#include <cmath>

#include "light.h"



using namespace std;

double DirectionalLight::distanceAttenuation( const Vec3d& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation( const Vec3d& P ) const
{
  Vec3d dir = getDirection(P);
  ray r = ray(P, dir, ray::SHADOW);
  isect i;
  Vec3d color = getColor(P);
  while(this->scene->intersect(r,i)) {
    if (!(i.t > 0)) {
      break;
    }
    if (i.t > RAY_EPSILON) {
      // check if object is see through, if so add its color to color
      Vec3d seethrough = i.getMaterial().kt(i);
      if (!seethrough[0] && !seethrough[1] && !seethrough[2]) {
        return Vec3d(0,0,0);
      }
      if (i.N * dir < 0) {
        color[0] *= seethrough[0];
        color[1] *= seethrough[1];
        color[2] *= seethrough[2];
      }
    }
    // throw another ray from t + RAY_EPSILON
    Vec3d newP = r.at(i.t + RAY_EPSILON);
    r = ray(newP, dir, ray::SHADOW);
  }
  return color;
 }

Vec3d DirectionalLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d DirectionalLight::getDirection( const Vec3d& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const Vec3d& P ) const
{

	// YOUR CODE HERE
  double d = (P - position).length();
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	double small = 1/(constantTerm + linearTerm * d + quadraticTerm * d * d);
  if (small > 1) {
    small = 1;
  }
  return small;
}

Vec3d PointLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d PointLight::getDirection( const Vec3d& P ) const
{
	Vec3d ret = position - P;
	ret.normalize();
	return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
  // YOUR CODE HERE:
  // You should implement shadow-handling code here.
  Vec3d dir = position - P;
  double tp = dir.length();
  dir.normalize();
  ray r = ray(P, dir, ray::SHADOW);
  isect i;
  Vec3d color = getColor(P);
  while(this->scene->intersect(r,i)) {
    if (i.t > tp || i.t < 0) {
      break;
    } else if (i.t > RAY_EPSILON) {
      Vec3d seethrough = i.getMaterial().kt(i);
      if (!seethrough[0] && !seethrough[1] && !seethrough[2]) {
        return Vec3d(0,0,0);
      }
      if (i.N * dir < 0) {
        color[0] *= seethrough[0];
        color[1] *= seethrough[1];
        color[2] *= seethrough[2];
      }
    }
    // throw another ray from t + RAY_EPSILON
    Vec3d newP = r.at(i.t + RAY_EPSILON);
    r = ray(newP, dir, ray::SHADOW);
  } 
  return color;
}
