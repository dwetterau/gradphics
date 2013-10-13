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
  if (root->leaf) {
    bool have_one = false;
	  typedef vector<Geometry*>::const_iterator iter;
    for( iter j = root->objects.begin(); j != root->objects.end(); ++j ) {
		  isect cur;
		  if( (*j)->intersect( r, cur ) ) {
        if((cur.t >= 0) && (!have_one || (cur.t < i.t))) {
				  i = cur;
				  have_one = true;
			  }
		  }
	  }
	  return have_one; 
  } else {
    // See which side(s) to recurse down
  	double tmin, tmax;
	  if (!root->bounds.intersect(r, tmin, tmax) || tmax < 0) {
      // Doesn't intersect empty space...
      return false;
    }
    // compute t*
    double d = root->d[0] + root->d[1] + root->d[2];
    d -= root->N * r.getPosition();
    double dot = root->N * r.getDirection();
    if (dot == 0) {
      dot = RAY_EPSILON;
    }
    double t = d / dot;
    
    if (t > tmax) {
      if (dot > 0) {
        return findFirstIntersection(r, i, root->back); 
      } else {
        return findFirstIntersection(r, i, root->front);
      }
    } else if (t < tmin) {
      if (dot > 0) {
        return findFirstIntersection(r, i, root->front);
      } else {
        return findFirstIntersection(r, i, root->back);
      }
    } else if (t <= tmax && t >= tmin) {
      if (dot > 0) {
        if (t < 0) {
          return findFirstIntersection(r, i, root->front);
        }
        isect cur;
        bool inFirst = findFirstIntersection(r, cur, root->back);
        if (!inFirst) {
          return findFirstIntersection(r, i, root->front);
        }
        i = cur;
        return true;
      } else {
        if (t < 0) {
          return findFirstIntersection(r, i, root->back);
        }
        isect cur;
        bool inFirst = findFirstIntersection(r, cur, root->front);
        if (!inFirst) {
          return findFirstIntersection(r, i, root->back);
        }
        i = cur;
        return true;
      }
    }
  }
  return false;
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const {
	bool have_one = false;
	typedef vector<Geometry*>::const_iterator iter;
  //TODO add UI for switching
  bool accelerated = true;
  if (accelerated) {
    if (findFirstIntersection(r, i, &root)) {
      intersectCache.push_back( std::make_pair(r,i) );
      return true; 
    } else {
      i.setT(1000.0);
      intersectCache.push_back( std::make_pair(r,i) );
      return false;
    }

  } else {
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
    std::vector<Geometry*>* front_objs, std::vector<Geometry*>* back_objs) {
  
  if (val >= bounds.getMin()[index] && val <= bounds.getMax()[index]) {
    double Sa = 0;
    double Sb = 0;
    int Na = 0;
    int Nb = 0;
    std::vector<Geometry*> temp_front_objs;
    std::vector<Geometry*> temp_back_objs;
    for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
      BoundingBox bb = objs[j]->getBoundingBox();
      double cur_area = bb.area();
      double cur_min = bb.getMin()[index];
      double cur_max = bb.getMax()[index];
      if (cur_min <= val) {
        Sa += cur_area;
        Na += 1;
        temp_back_objs.push_back(objs[j]);
      }
      if (cur_max >= val) {
        Sb += cur_area;
        Nb += 1;
        temp_front_objs.push_back(objs[j]);
      }
    }
    double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
    if (C < bestC) {
      *front_objs = std::vector<Geometry*>(temp_front_objs);
      *back_objs = std::vector<Geometry*>(temp_back_objs);
      this->N = Vec3d(0, 0, 0);
      this->d = Vec3d(0, 0, 0);
      this->N[index] = 1;
      this->d[index] = val;

      Vec3d newMaxBounds = bounds.getMax();
      newMaxBounds[index] = val;
      Vec3d newMinBounds = bounds.getMin();
      newMinBounds[index] = val;
      this->back_bb = BoundingBox(bounds.getMin(), newMaxBounds);
      this->front_bb = BoundingBox(newMinBounds, bounds.getMax());
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
  double S = bounds.area(); 
  std::vector<Geometry*> best_front_objs;
  std::vector<Geometry*> best_back_objs;
  double bestC = 2000000000;
  BoundingBox front;
  BoundingBox back;

  for (std::vector<Geometry*>::size_type i = 0; i < objs.size(); i++) {
    // bad n^2 algorithm for this
    std::vector<Geometry*>* front_objs = new std::vector<Geometry*>(); 
    std::vector<Geometry*>* back_objs = new std::vector<Geometry*>(); 
   
    // xMin for this object:
    double val = objs[i]->getBoundingBox().getMin()[0];
    double C = tryPlane(val, 0, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
    val = objs[i]->getBoundingBox().getMax()[0];
    C = tryPlane(val, 0, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
    
    val = objs[i]->getBoundingBox().getMin()[1];
    C = tryPlane(val, 1, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
    val = objs[i]->getBoundingBox().getMax()[1];
    C = tryPlane(val, 1, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
    
    val = objs[i]->getBoundingBox().getMin()[2];
    C = tryPlane(val, 2, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
    val = objs[i]->getBoundingBox().getMax()[2];
    C = tryPlane(val, 2, objs, S, bestC, front_objs, back_objs);
    if (C < bestC) {
      bestC = C;
      best_front_objs = std::vector<Geometry*>(*front_objs);
      best_back_objs = std::vector<Geometry*>(*back_objs);
    }
  }
  this->front = new kdNode(front_bb);
  this->back = new kdNode(back_bb);
  this->front->fill(best_front_objs, depth + 1);
  this->back->fill(best_back_objs, depth + 1);
}

