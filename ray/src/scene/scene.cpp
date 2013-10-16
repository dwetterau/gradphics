#include <cmath>
#include <algorithm>

#include "scene.h"
#include "light.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;

bool Geometry::intersect(const ray&r, isect&i) const {
	double tmin, tmax;
	if (hasBoundingBoxCapability() && !(bounds.intersect(r, tmin, tmax))) return false;
	// Transform the ray into the object's local coordinate space
	Vec3d pos = transform->globalToLocalCoords(r.getPosition());
	Vec3d dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
	double length = dir.length();
	dir /= length;

	ray localRay( pos, dir, r.type() );

	if (intersectLocal(localRay, i)) {
		// Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;
		return true;
	} else return false;
}

bool Geometry::hasBoundingBoxCapability() const {
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	return false;
}

Scene::~Scene() {
    giter g;
    liter l;
	tmap::iterator t;
	for( g = objects.begin(); g != objects.end(); ++g ) delete (*g);
	for( l = lights.begin(); l != lights.end(); ++l ) delete (*l);
	for( t = textureCache.begin(); t != textureCache.end(); t++ ) delete (*t).second;
}

bool Scene::findFirstIntersection( const ray& r, isect& i, const kdNode* root, double tmin, double tmax) const {
  if (tmax < 0) {
    return false;
  }
  if (root->leaf) {
    bool have_one = false;
	  typedef vector<Geometry*>::const_iterator iter;
    for( iter j = root->objects.begin(); j != root->objects.end(); ++j ) {
		  isect cur;
		  if( (*j)->intersect( r, cur ) ) {
        if (cur.t < tmin || cur.t > tmax) {
          continue;
        }
        if((cur.t >= 0) && (!have_one || (cur.t < i.t))) {
				  i = cur;
				  have_one = true;
			  }
		  }
	  }
	  return have_one;
  } else {
    tmax = min(tmax, 1000.0);
    // See which side(s) to recurse down	
    // compute t*
    
    double d = root->d[0] + root->d[1] + root->d[2];
    d -= root->N * r.getPosition();
    double dot = root->N * r.getDirection();
    if (dot == 0) {
      dot = RAY_EPSILON;
    }
    double t = d / dot;
   
    kdNode* front = root->front;
    kdNode* back = root->back;

    if (dot < 0) {
     back = root->front;
     front = root->back;
    }
    if (t > tmax) {
      return findFirstIntersection(r, i, back, tmin, tmax); 
    } else if (t < tmin) {
      return findFirstIntersection(r, i, front, tmin, tmax);
    } else if (t <= tmax && t >= tmin) {
      if (t < 0) {
        return findFirstIntersection(r, i, front, t, tmax);
      }
      isect cur;
      bool first = findFirstIntersection(r, cur, back, tmin, t);
      if (!first) {
        return findFirstIntersection(r, i, front, t, tmax);
      } else {
        i = cur;
        return true;
      }
    } 
  }
  return false;
}

bool Scene::shouldAccelerate() const{
  return traceUI->getAccelerated();
}

bool Scene::cubeMap() const {
  return traceUI->getCubeMap() && xpMap;
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const {
  if (shouldAccelerate()) {
  	double tmin, tmax;
	  if (!root.bounds.intersect(r, tmin, tmax)) {
      return false;
    }
    if (findFirstIntersection(r, i, &root, tmin, tmax)) {
      intersectCache.push_back( std::make_pair(r,i) );
      return true; 
    } else {
      i.setT(1000.0);
      intersectCache.push_back( std::make_pair(r,i) );
      return false;
    }
  } else {
    bool have_one = false;
	  typedef vector<Geometry*>::const_iterator iter;
    for( iter j = objects.begin(); j != objects.end(); ++j ) {
		  isect cur;
		  if( (*j)->intersect( r, cur ) ) {
			  if( !have_one || (cur.t < i.t) ) {
				  i = cur;
				  have_one = true;
			  }
		  }
	  }
	  if( !have_one ) i.setT(1000.0);
	  // if debugging,
	  intersectCache.push_back( std::make_pair(r,i) );
	  return have_one;
  }
}

TextureMap* Scene::getTexture( string name ) {
	tmap::const_iterator itr = textureCache.find( name );
	if( itr == textureCache.end() ) {
		textureCache[ name ] = new TextureMap( name );
		return textureCache[ name ];
	} else return (*itr).second;
}

void Scene::loadCubeMap() {
  if (traceUI->getCubeMap()) {
    cout << "loaded the cube map" << endl;
    string textureName = "/home/david/Dropbox/384g/ray/scenes/textry.png";
    xpMap = getTexture(textureName);
    xnMap = getTexture(textureName);
    ypMap = getTexture(textureName);
    ynMap = getTexture(textureName);
    zpMap = getTexture(textureName);
    znMap = getTexture(textureName);
  }
}

void Scene::buildKdTree() {
  Vec3d min = bounds().getMin();
  Vec3d max = bounds().getMax();
  min[0] -= RAY_EPSILON;
  min[1] -= RAY_EPSILON;
  min[2] -= RAY_EPSILON;
  max[0] += RAY_EPSILON;
  max[1] += RAY_EPSILON;
  max[2] += RAY_EPSILON;
  root = kdNode(BoundingBox(min, max));
  root.fill(objects, 0);
  cout << "finished filling tree with " << objects.size() << " objects" << endl;
}

kdNode::kdNode(const BoundingBox& bb) {
  bounds = BoundingBox(bb.getMin(), bb.getMax()); 
}

double kdNode::tryPlane(double val, int index, std::vector<Geometry*> objs, 
    double S, double bestC, 
    std::vector<Geometry*>* front_objs, 
    std::vector<Geometry*>* back_objs,
    Vec3d* d,
    Vec3d* N,
    BoundingBox* front_bb,
    BoundingBox* back_bb) {
  
  double Sa = 0;
  double Sb = 0;
  int Na = 0;
  int Nb = 0;
  if (val > bounds.getMin()[index] && val < bounds.getMax()[index]) {
    for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
      BoundingBox bb = objs[j]->getBoundingBox();
      double cur_min = bb.getMin()[index];
      double cur_max = bb.getMax()[index];
      if (cur_min < val + RAY_EPSILON) {
        Na += 1;
        Sa += bb.area();
      }
      if (cur_max > val - RAY_EPSILON) {
        Nb += 1;
        Sb += bb.area();
      }
    }
    double C = kt + ki * (Na * (Sa / S) + Nb * (Sb / S));
    if (C < bestC) {
      // Need to do the loop again to actually get the list of objects
      std::vector<Geometry*> temp_front_objs = std::vector<Geometry*>();
      std::vector<Geometry*> temp_back_objs = std::vector<Geometry*>();
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_min = bb.getMin()[index];
        double cur_max = bb.getMax()[index];
        if (cur_min < val) {
          temp_back_objs.push_back(objs[j]);
        }
        if (cur_max > val) {
          temp_front_objs.push_back(objs[j]);
        }
      } 
      Vec3d newMaxBounds = bounds.getMax();
      newMaxBounds[index] = val;
      Vec3d newMinBounds = bounds.getMin();
      newMinBounds[index] = val;
      BoundingBox back = BoundingBox(bounds.getMin(), newMaxBounds);
      BoundingBox front = BoundingBox(newMinBounds, bounds.getMax()); 
      
      *front_objs = temp_front_objs; //std::vector<Geometry*>(temp_front_objs);
      *back_objs = temp_back_objs; //std::vector<Geometry*>(temp_back_objs);
      *N = Vec3d(0, 0, 0);
      *d = Vec3d(0, 0, 0);
      (*N)[index] = 1;
      (*d)[index] = val;
      *back_bb = back;
      *front_bb = front;
    }
    return C;
  } else {
    return 1e308;
  }
}

void kdNode::fill(std::vector<Geometry*> objs, int depth) {
  if (depth >= depthLimit || objs.size() <= objectLimit) {
    this->objects = objs;
    this->leaf = true;
    return;
  }
  this->leaf = false;
  // try splitting on all x values
  // first sort objects based off xMin
  this->front = NULL;
  double totalS = 0;
  for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
    BoundingBox bb = objs[j]->getBoundingBox();
    totalS += bb.area();
  }
  double S = totalS; //bounds.area(); 
  std::vector<Geometry*> best_front_objs;
  std::vector<Geometry*> best_back_objs;
  double bestC = 1e308;

  for (std::vector<Geometry*>::size_type i = 0; i < objs.size(); i++) {
    // bad n^2 algorithm for this
    std::vector<Geometry*>* front_objs = new std::vector<Geometry*>(); 
    std::vector<Geometry*>* back_objs = new std::vector<Geometry*>(); 
    Vec3d* cur_N = new Vec3d(0,0,0);
    Vec3d* cur_d = new Vec3d(0,0,0);
    BoundingBox* cur_back_bb  = new BoundingBox();
    BoundingBox* cur_front_bb  = new BoundingBox();

    // xMin for this object:
    double val, C;
    for (int j = 0; j < 3; j++) {
      val = objs[i]->getBoundingBox().getMin()[j] - RAY_EPSILON;
      C = tryPlane(val, j, objs, S, bestC, front_objs, back_objs, cur_d, cur_N, cur_front_bb, cur_back_bb);
      if (C < bestC) {
        bestC = C;
        best_front_objs = std::vector<Geometry*>(*front_objs);
        best_back_objs = std::vector<Geometry*>(*back_objs);
        this->N = *cur_N;
        this->d = *cur_d;
        this->front = new kdNode(*cur_front_bb);
        this->back = new kdNode(*cur_back_bb);
      }
      val = objs[i]->getBoundingBox().getMax()[j] + RAY_EPSILON;
      C = tryPlane(val, j, objs, S, bestC, front_objs, back_objs, cur_d, cur_N, cur_front_bb, cur_back_bb);
      if (C < bestC) {
        bestC = C;
        best_front_objs = std::vector<Geometry*>(*front_objs);
        best_back_objs = std::vector<Geometry*>(*back_objs);
        this->N = *cur_N;
        this->d = *cur_d;
        this->front = new kdNode(*cur_front_bb);
        this->back = new kdNode(*cur_back_bb);
      }
    }
  }
  if (bestC == 1e308 || best_front_objs.size() + best_back_objs.size() > 1.5 * objs.size()) {
    // This node should be a leaf, no good plane to split on
    this->objects = objs;
    this->leaf = true;
    return;
  }
  this->front->fill(std::vector<Geometry*>(best_front_objs), depth + 1);
  this->back->fill(std::vector<Geometry*>(best_back_objs), depth + 1);
}

