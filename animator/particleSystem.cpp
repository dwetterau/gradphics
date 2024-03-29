#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#define DELTA 100
#define IDELTA 0.01

using namespace std;

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
  particles = vector<vector<Particle> >();
  time_to_index = map<int, int>();
  forces = vector<Force*>();
}

/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
  delete &particles;
  delete &time_to_index;
  delete &forces;
}

/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
  resetSimulation(t);
  prevT = t;
	// These values are used by the UI ...
	// negativebake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
  bake_start_time = t;
	bake_end_time = -1;
	simulate = true;
	dirty = true;
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
  bake_end_time = t;
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
  clearBaked();
  vector<Particle> newParts = initialFill();
  particles.push_back(newParts);
	time_to_index[t] = 0;
  // These values are used by the UI
	simulate = false;
	dirty = true;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
  if (!simulate || t < bake_start_time) {
    return;
  }
  if (particles.size() == 0) {
    return;
  }
  //prevT = .16667
  // t = .17
  t = t + IDELTA / 2;
  if (int(t * DELTA) != int(prevT * DELTA)) {
    vector<Particle> newParticles = initialFill();
    // determine the right thing to grab
    if (time_to_index.find(int(t * DELTA)) == time_to_index.end()) {
      // not in here, we need to find the prevT to use
      int max = -1;
      for(map<int, int>::iterator iter = time_to_index.begin(); iter != time_to_index.end(); ++iter) {
        if (iter->first > max && iter->first < int(t * DELTA)) {
          max = iter->first;
        }
      }
      if (max <= int(t * DELTA - 1)) {
        computeForcesAndUpdateParticles(t - IDELTA);
      }
    } else {
      return;
    }
    int prevIndex = int(t * DELTA - 1);
    int index = time_to_index[prevIndex];
    for (int i = 0; i < particles[index].size(); i++) {
      Particle p = particles[index][i];
      p.update(IDELTA);
      if (p.lifespan > 0) {
        newParticles.push_back(p);
      }
    }
    applyForces(newParticles);
    bakeParticles(t, newParticles);
    prevT = t;
  }
}

void ParticleSystem::applyForces(vector<Particle>& p) {
  for (int i = 0; i < p.size(); i++) {
    if (p[i].rad <= 0.00000001) {
      continue;
    }
    p[i].f = Vec3d(0,0,0);
    for (int j = 0; j < forces.size(); j++) {
      forces[j]->apply(p[i]);
    }
  }
}

/** Render particles */
void ParticleSystem::drawParticles(float t)
{
  if (particles.size() == 0) {
    return;
  }
  if (time_to_index.find(int(t * DELTA)) == time_to_index.end()) {
    return;
  }
  for (int j = 0; j < forces.size(); j++) {
    forces[j]->buildVectors();
  }
  int i = time_to_index[int(t * DELTA)];
  vector<Particle> curPs = particles[i];
  drawParts(curPs);
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t, vector<Particle> newbs) 
{
  int i = particles.size();
  int index = int(t * DELTA);
  time_to_index[index] = i;
  particles.push_back(newbs);
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
  time_to_index.clear();
  particles.clear();
}
