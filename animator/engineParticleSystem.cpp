#include "engineParticleSystem.h"
#include "forces.h"

#define VAR 5.0
#define HVAR VAR / 2
#define R 0.03
#define M 10.0
#define L .2
#define V 5.0

using namespace std;

EngineSystem::EngineSystem() {
  prevT = 0.0;
  Force f = Gravity(M);
  forces = vector<Force>();
  forces.push_back(f);
}

vector<Particle> EngineSystem::initialFill() {
  
  Vec4f npo = glMat * Vec4f(0,0,0,1);
  Vec3d po = Vec3d(npo[0], npo[1], npo[2]);
  Vec4f nv = glMat * Vec4f(0,0,V,0);
  
  vector<Particle> toReturn = vector<Particle>();
  Vec3d f = Vec3d(0,0,0);
  Vec3d c = Vec3d(1,0,0);
  for (int i = 0; i < getPc(); i++) {
    float dvx = VAR * ((float)rand()/(float)RAND_MAX) - HVAR;
    float dvy = VAR * ((float)rand()/(float)RAND_MAX) - HVAR; 
    float dvz = VAR * ((float)rand()/(float)RAND_MAX) - HVAR; 
    Particle p = Particle(po, Vec3d(nv[0] + dvx, nv[1] + dvy, nv[2] + dvz), f, c, R, M, L); 
    toReturn.push_back(p);
  }
  return toReturn;
}
