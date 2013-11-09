#include "engineParticleSystem.h"
#include "forces.h"

#define VAR .2
#define HVAR VAR / 2
#define R 0.1
#define M 0.1
#define L 1.0


using namespace std;

EngineSystem::EngineSystem() {
  Force f = Gravity();
  forces = vector<Force>();
  forces.push_back(f);
}

vector<Particle> EngineSystem::initialFill() {
  vector<Particle> toReturn = vector<Particle>();
  Vec3d f = Vec3d(0,0,0);
  Vec3d po = Vec3d(0,0,0);
  Vec3d c = Vec3d(1,0,0);
  for (int i = 0; i < getPc(); i++) {
    float dvx = VAR * ((float)rand()/(float)RAND_MAX) - HVAR;
    float dvy = VAR * ((float)rand()/(float)RAND_MAX) - HVAR; 
    float dvz = 1 + VAR * ((float)rand()/(float)RAND_MAX) - HVAR; 
    Particle p = Particle(po, Vec3d(dvx, dvy, dvz), f, c, R, M, L); 
    toReturn.push_back(p);
  }
  return toReturn;
}
