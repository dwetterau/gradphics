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

static float prevT;

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
  particles = vector<vector<Particle> >();
  time_to_index = map<int, int>();
  forces = vector<Force>();
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
	// TODO
  resetSimulation(t);
  prevT = t;
	// These values are used by the UI ...
	// negative bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	// TODO
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
  if (particles.size() == 0) {
    return;
  }
  if (t != prevT) {
    vector<Particle> newParticles = initialFill();
    // determine the right thing to grab
    if (time_to_index.find(int(t * DELTA)) == time_to_index.end()) {
      // not in here, we need to find the prevT to use
      int max = -1;
      for(map<int, int>::iterator iter = time_to_index.begin(); iter != time_to_index.end(); ++iter) {
        if (iter->first > max && iter->first < t) {
          max = iter->first;
        }
      }
      if (max < int((t - IDELTA) * DELTA)) {
        computeForcesAndUpdateParticles(t - IDELTA);
      }
    } else {
      return;
    }
    int prevIndex = int((t - IDELTA) * DELTA);
    cout << "This t=" << t << " Looking at entry: " << time_to_index[prevIndex] << endl;
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
    cout << "We did it! " << t << endl;
    prevT = t;
  }
}

void ParticleSystem::applyForces(vector<Particle>& p) {
  for (int i = 0; i < forces.size(); i++) {
    forces[i].updateVectors(glMat);
  }
  
  for (int i = 0; i < p.size(); i++) {
    p[i].f = Vec3d(0,0,0);
    for (int j = 0; j < forces.size(); j++) {
      forces[j].apply(p[i]);
    }
  }
}

/** Render particles */
void ParticleSystem::drawParticles(float t)
{
  if (particles.size() == 0) {
    return;
  }
  int i = time_to_index[int(t * DELTA)];
  vector<Particle> curPs = particles[i];
  for (int j = 0; j < curPs.size(); j++) {
    Particle p = curPs[j];
    setDiffuseColor(p.c[0], p.c[1], p.c[2]);
    setAmbientColor(p.c[0], p.c[1], p.c[2]);
    glPushMatrix();
    glTranslatef(p.p[0], p.p[1], p.p[2]);
    drawSphere(p.rad);
    glPopMatrix();
  }
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t, vector<Particle> newbs) 
{
  int i = particles.size();
  time_to_index[int(t * DELTA)] = i;
  particles.push_back(newbs);
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
  time_to_index.clear();
  particles.clear();
}
