/***********************
 * ParticleSystem class
 ***********************/

/**
 * The particle system class simply "manages" a collection of particles.
 * Its primary responsibility is to run the simulation, evolving particles
 * over time according to the applied forces using Euler's method.
 * This header file contains the functions that you are required to implement.
 * (i.e. the rest of the code relies on this interface)
 * In addition, there are a few suggested state variables included.
 * You should add to this class (and probably create new classes to model
 * particles and forces) to build your system.
 */

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "vec.h"
#include "mat.h"
#include <vector>
#include <map>

class Particle {
  public:  
    Vec3d p;
    Vec3d v;
    Vec3d f;
    Vec3d c;
    
    Vec3d d;
    
    double rad;
    double m;
    double lifespan;

    Particle(Vec3d _p, Vec3d _v, Vec3d _f, Vec3d _c, double r, double mass, double l) : p(_p), v(_v), f(_f), c(_c), rad(r), m(mass), lifespan(l) {
      d = Vec3d(0,0,0);
    }

    void update(float dt) {
      lifespan -= dt;
      move(dt);
    }
    
    void move(float dt) {
        p = p + v * dt;
        v = v + f * (dt / m);
    }
};

class Force {
  public: 
    std::vector<Vec3d> vectors;

    virtual void apply(Particle& p) = 0; 
    virtual void buildVectors() = 0;
};


class ParticleSystem {

public:
  float prevT;
  std::map<int, int> time_to_index;
  std::vector<std::vector<Particle> > particles;
  std::vector<Force*> forces;
  Mat4f glMat; 
  float pc;

  virtual void setVel(Vec4f v) = 0;
  virtual void applyForces(std::vector<Particle>& p);

	/** Constructor **/
	ParticleSystem();

	/** Destructor **/
	virtual ~ParticleSystem();

	/** Simulation fxns **/
	// This fxn should render all particles in the system,
	// at current time t.
	void drawParticles(float t);
	virtual void drawParts(std::vector<Particle> curPs) = 0;

	// This fxn should save the configuration of all particles
	// at current time t.
	virtual void bakeParticles(float t, std::vector<Particle> idgaf);

	// This function should compute forces acting on all particles
	// and update their state (pos and vel) appropriately.
	virtual void computeForcesAndUpdateParticles(float t);

	// This function should reset the system to its initial state.
	// When you need to reset your simulation, PLEASE USE THIS FXN.
	// It sets some state variables that the UI requires to properly
	// update the display.  Ditto for the following two functions.
	virtual void resetSimulation(float t);

	// This function should start the simulation
	virtual void startSimulation(float t);

	// This function should stop the simulation
	virtual void stopSimulation(float t);

  // subclasses should fill the initial positions / velocities of the points
  virtual std::vector<Particle> initialFill() = 0;

	// This function should clear out your data structure
	// of baked particles (without leaking memory).
	virtual void clearBaked();	


	// These accessor fxns are implemented for you
	float getBakeStartTime() { return bake_start_time; }
	float getBakeEndTime() { return bake_end_time; }
	float getBakeFps() { return bake_fps; }
	bool isSimulate() { return simulate; }
	bool isDirty() { return dirty; }
	void setDirty(bool d) { dirty = d; }
  float getPc() {
    return pc;
  }
  void setPc(float p) {
    pc = p;
  }

protected:
	

	/** Some baking-related state **/
	float bake_fps;						// frame rate at which simulation was baked
	float bake_start_time;				// time at which baking started 
										// These 2 variables are used by the UI for
										// updating the grey indicator 
	float bake_end_time;				// time at which baking ended

	/** General state variables **/
	bool simulate;						// flag for simulation mode
	bool dirty;							// flag for updating ui (don't worry about this)

};


#endif	// __PARTICLE_SYSTEM_H__
