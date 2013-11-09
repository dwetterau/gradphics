#ifndef FORCES_H
#define FORCES_H

#include "particleSystem.h"

class Gravity : public Force {
  public:
    typedef Force super;
    Gravity() {
      global_vectors = std::vector<Vec3d>();
      global_vectors.push_back(Vec3d(0, -.01, 0));
    }
};

#endif
