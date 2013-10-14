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

bool Scene::findFirstIntersection( const ray& r, isect& i, const kdNode* root) const {
  double tmin, tmax;
	if (!root->bounds.intersect(r, tmin, tmax)) {
    // Doesn't intersect empty space...
    return false;
  }
  if (tmax < 0) {
    return 0;
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
    tmin = max(tmin, 0.0);
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
      return findFirstIntersection(r, i, back); 
    } else if (t < tmin) {
      return findFirstIntersection(r, i, front);
    } else if (t <= tmax && t >= tmin) {
      if (t < 0) {
        return findFirstIntersection(r, i, front);
      }
      return findFirstIntersection(r, i, back) || findFirstIntersection(r, i, front);
    }
  }
  return false;
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const {
	bool accelerated = traceUI->getAccelerated();
  if (accelerated) {
    if (findFirstIntersection(r, i, &root)) {
      //intersectCache.push_back( std::make_pair(r,i) );
      return true; 
    } else {
      //i.setT(1000.0);
      //intersectCache.push_back( std::make_pair(r,i) );
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

void Scene::buildKdTree() {
  root = kdNode(bounds());
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
  
  if (val > bounds.getMin()[index] && val < bounds.getMax()[index]) {
    double Sa = 0;
    double Sb = 0;
    int Na = 0;
    int Nb = 0;
    std::vector<Geometry*> temp_front_objs = std::vector<Geometry*>();
    std::vector<Geometry*> temp_back_objs = std::vector<Geometry*>();
    for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
      BoundingBox bb = objs[j]->getBoundingBox();
      double cur_area = bb.area();
      double cur_min = bb.getMin()[index];
      double cur_max = bb.getMax()[index];
      if (cur_min < val) {
        Sa += cur_area;
        Na += 1;
        temp_back_objs.push_back(objs[j]);
      }
      if (cur_max > val) {
        Sb += cur_area;
        Nb += 1;
        temp_front_objs.push_back(objs[j]);
      }
    }
    if (Na + Nb > 3.0 * objs.size() / 2.0) {
      return 2000000000;
    }
    double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
    if (C < bestC) {
      *front_objs = std::vector<Geometry*>(temp_front_objs);
      *back_objs = std::vector<Geometry*>(temp_back_objs);
      *N = Vec3d(0, 0, 0);
      *d = Vec3d(0, 0, 0);
      (*N)[index] = 1;
      (*d)[index] = val;

      Vec3d newMaxBounds = bounds.getMax();
      newMaxBounds[index] = val;
      Vec3d newMinBounds = bounds.getMin();
      newMinBounds[index] = val;
      *back_bb = BoundingBox(bounds.getMin(), newMaxBounds);
      *front_bb = BoundingBox(newMinBounds, bounds.getMax());
    }
    return C;
  } else {
    return 2000000000;
  }
}

void kdNode::fill(std::vector<Geometry*> objs, int depth) {
  if (depth >= depthLimit || objs.size() <= objectLimit) {
    cout << "made node with # " << objs.size() << endl;
    this->objects = objs;
    this->leaf = true;
    return;
  }
  this->leaf = false;
  // try splitting on all x values
  // first sort objects based off xMin
  this ->front = NULL;
  double totalS = 0;
  for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
    BoundingBox bb = objs[j]->getBoundingBox();
    totalS += bb.area();
  }
  double S = bounds.area(); 
  std::vector<Geometry*> best_front_objs;
  std::vector<Geometry*> best_back_objs;
  double bestC = ki * totalS / S * objs.size();

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
      val = objs[j]->getBoundingBox().getMin()[j];
      C= tryPlane(val, j, objs, S, bestC, front_objs, back_objs, cur_d, cur_N, cur_front_bb, cur_back_bb);
      if (C < bestC) {
        bestC = C;
        best_front_objs = std::vector<Geometry*>(*front_objs);
        best_back_objs = std::vector<Geometry*>(*back_objs);
        this->N = *cur_N;
        this->d = *cur_d;
        this->front = new kdNode(*cur_front_bb);
        this->back = new kdNode(*cur_back_bb);
      }
      val = objs[j]->getBoundingBox().getMax()[j];
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
  if (!this->front) {
    // This node should be a leaf, no good plane to split on
    cout << "made special node with # " << objs.size() << endl;
    this->objects = objs;
    this->leaf = true;
    return;
  }
  this->front->fill(std::vector<Geometry*>(best_front_objs), depth + 1);
  this->back->fill(std::vector<Geometry*>(best_back_objs), depth + 1);
}

