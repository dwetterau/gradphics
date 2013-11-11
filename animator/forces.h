#ifndef FORCES_H
#define FORCES_H

#define G 10.0
#define K 10.0

#include "particleSystem.h"

class Gravity : public Force {
  public:
    float m; 
    float g;
    Gravity(float _m, float _g) : m(_m), g(_g) {
      vectors = std::vector<Vec3d>();
      vectors.push_back(Vec3d(0, -g * m, 0));
    }
};

class Drag : public Force {
  public:
    float k;
    Drag(float _k) : k(_k) {
     
    }

    void apply(Particle& p) {
      p.f += -k * p.v;
    }
};

#endif
