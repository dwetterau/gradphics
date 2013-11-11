#ifndef LASER_PS_H
#define LASER_PS_H

#include "particleSystem.h"


class LaserSystem : public ParticleSystem {
  public:

    LaserSystem();
    std::vector<Particle> initialFill();
    void drawParts(std::vector<Particle> curPs);
};

#endif
