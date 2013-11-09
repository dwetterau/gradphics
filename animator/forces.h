#ifndef FORCES_H
#define FORCES_H

#include "particleSystem.h"

class Gravity : public Force {
  public:
    float m; 
    Gravity(float _m) : m(_m) {
      vectors = std::vector<Vec3d>();
      vectors.push_back(Vec3d(0, -.5 * m, 0));
    }
};

#endif
