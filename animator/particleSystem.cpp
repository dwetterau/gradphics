#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>

#define DELTA 100

using namespace std;

static float prevT;

/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
  particles = vector<vector<Particle> >();
  time_to_index = map<float, int>();
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
  t = roundf(t * DELTA) / DELTA;
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
  particles.push_back(initialFill());
	time_to_index[t] = 0;
  // These values are used by the UI
	simulate = false;
	dirty = true;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
  t = roundf(t * DELTA) / DELTA;
  if (t != prevT) {
    vector<Particle> newParticles = initialFill();
    for (int i = 0; i < particles[particles.size() - 1].size(); i++) {
      Particle p = particles[particles.size() - 1][i];
      p.update(t - prevT);
      if (p.lifespan > 0) {
        newParticles.push_back(p);
      }
    }
    applyForces(newParticles);
    bakeParticles(t, newParticles);
  }

	// Debugging info
	if( t - prevT > .04 )
		printf("(!!) Dropped Frame %lf (!!)\n", t-prevT);
	prevT = t;
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
  t = roundf(t * DELTA) / DELTA;
  int i = time_to_index[t];
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
  time_to_index[t] == i;
  particles.push_back(newbs);
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
  time_to_index.clear();
  particles.clear();
}
