#ifndef ENGINE_PS_H
#define ENGINE_PS_H

#include "particleSystem.h"


class EngineSystem : public ParticleSystem {
  public:

    EngineSystem();
    std::vector<Particle> initialFill(); 
};

#endif
