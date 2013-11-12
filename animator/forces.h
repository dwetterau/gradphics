#ifndef FORCES_H
#define FORCES_H

#include "particleSystem.h"

class Gravity : public Force {
  public:
    float m; 
    static float g;
    Gravity(float _m) : m(_m) {}

    void buildVectors() {
      vectors = std::vector<Vec3d>();
      vectors.push_back(Vec3d(0, -g * m, 0));
    }

    void apply(Particle& p) {
      for (int i = 0; i < vectors.size(); i++) {
        p.f += vectors[i];
      }
    }
};


class Drag : public Force {
  public:
    static float k;
    Drag() {}

    void apply(Particle& p) {
      p.f += -k * p.v;
    }

    void buildVectors() {
    }
};

static void setDrag(float _k) { Drag::k = _k; }
static void setGravity(float _g) { Gravity::g = _g; }
#endif
