#include "laserSystem.h"
#include "forces.h"
#include "modelerdraw.h"

#define VAR 5.0
#define HVAR VAR / 2
#define R 0.03
#define M 10.0
#define L 1.0
#define V 10.0

#define ML .5
#define PROB .1
#define THICK .08

using namespace std;

LaserSystem::LaserSystem() {
  prevT = 0.0;
  Force* f = new Gravity(M);
  Force* d = new Drag();
  forces = vector<Force*>();
  forces.push_back(f);
  forces.push_back(d);
}

vector<Particle> LaserSystem::initialFill() {
  Vec4f npo = glMat * Vec4f(0,0,0,1);
  Vec3d po = Vec3d(npo[0], npo[1], npo[2]);
  Vec4f nv = glMat * Vec4f(0,0,V,0);
  
  vector<Particle> toReturn = vector<Particle>();
  Vec3d f = Vec3d(0,0,0);
  Vec3d c = Vec3d(1,.1,0);
  int numToDraw = getPc() < 1.0 ? (((float)rand()/(float)RAND_MAX) <= getPc() ? 1 : 0) : int(getPc() + .05);
  for (int i = 0; i < numToDraw; i++) {
    bool bad = ((float)rand()/(float)RAND_MAX) <= PROB;
    if (bad) { 
      float dvx = VAR * ((float)rand()/(float)RAND_MAX) - HVAR;
      float dvy = VAR * ((float)rand()/(float)RAND_MAX) - HVAR; 
      float dvz = VAR * ((float)rand()/(float)RAND_MAX) - HVAR;
      Particle p = Particle(po, Vec3d(dvx, dvy, dvz), f, c, 1.0, M, L); 
      toReturn.push_back(p);
    } else {
      Particle p = Particle(po, Vec3d(nv[0], nv[1], nv[2]), f, c, 0.0, M, L); 
      toReturn.push_back(p);
    }
  }
  return toReturn;
}

void LaserSystem::drawParts(vector<Particle> curPs) {
  for (int j = 0; j < curPs.size(); j++) {
    Particle p = curPs[j];
    setDiffuseColor(p.c[0], p.c[1], p.c[2]);
    setAmbientColor(p.c[0], p.c[1], p.c[2]);
    glPushMatrix();
    glTranslatef(p.p[0], p.p[1], p.p[2]);
    if (p.rad > 0.0) {
      drawSphere(THICK);
      glPopMatrix();
      continue;
    }
    // determine xy plane rot
    double x = p.v[0];
    double y = p.v[1];
    double z = p.v[2];
    double len = sqrt(x*x + y*y);
    double ang = acos(x / len);
    if (y < 0) {
      ang = -ang;
    }
    glRotatef(ang / M_PI * 180.0, 0, 0, 1.0);
    // z plane rot
    ang = atan(z / len);
    glRotatef(-(ang / M_PI * 180.0), 0, 1.0, 0);
    // translate back into position
    double l = p.v.length() / 2;
    glTranslatef(-l/2, -THICK/2, -THICK/2);
    drawBox(l, THICK, THICK);
    
    //drawSphere(.1);
    glPopMatrix();
  }
}
