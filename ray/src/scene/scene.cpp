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

void kdNode::fill(std::vector<Geometry*> objs, int depth) {
  if (depth >= depthLimit || objs.size() <= objectLimit) {
    cout << "made node with # " << objs.size() << endl;
    this->objects = objs;
    this->leaf = true;
    return;
  }
  this->leaf = false;
  int kt = 2;
  int ki = 80 * kt;
  
  // try splitting on all x values
  // first sort objects based off xMin
   double S = bounds.area(); 
  std::vector<Geometry*> a_objs;
  std::vector<Geometry*> b_objs;
  double bestC = 2000000000;
  BoundingBox front;
  BoundingBox back;

  for (std::vector<Geometry*>::size_type i = 0; i < objs.size(); i++) {
    // bad n^2 algorithm for this
   
    // xMin for this object:
    double x_line = objs[i]->getBoundingBox().getMin()[0];
    if (x_line > bounds.getMin()[0] && x_line < bounds.getMax()[0]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_x_min = bb.getMin()[0];
        double cur_x_max = bb.getMax()[0];
        if (cur_x_min <= x_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_x_max >= x_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(1, 0, 0);
        d = Vec3d(x_line, 0, 0);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[0] = x_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[0] = x_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }
    
    //x_max
    x_line = objs[i]->getBoundingBox().getMax()[0];
    if (x_line > bounds.getMin()[0] && x_line < bounds.getMax()[0]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_x_min = bb.getMin()[0];
        double cur_x_max = bb.getMax()[0];
        if (cur_x_min <= x_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_x_max >= x_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(1, 0, 0);
        d = Vec3d(x_line, 0, 0);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[0] = x_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[0] = x_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }
    
    // y_min for this object:
    double y_line = objs[i]->getBoundingBox().getMin()[1];
    if (y_line > bounds.getMin()[1] && y_line < bounds.getMax()[1]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_y_min = bb.getMin()[1];
        double cur_y_max = bb.getMax()[1];
        if (cur_y_min <= y_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_y_max >= y_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(0, 1, 0);
        d = Vec3d(0, y_line, 0);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[1] = y_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[1] = y_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }
    
    //y_max
    y_line = objs[i]->getBoundingBox().getMax()[1];
    if (y_line > bounds.getMin()[1] && y_line < bounds.getMax()[1]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_y_min = bb.getMin()[1];
        double cur_y_max = bb.getMax()[1];
        if (cur_y_min <= y_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_y_max >= y_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(0, 1, 0);
        d = Vec3d(0, y_line, 0);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[1] = y_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[1] = y_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }

    // z_min for this object:
    double z_line = objs[i]->getBoundingBox().getMin()[2];
    if (z_line > bounds.getMin()[2] && z_line < bounds.getMax()[2]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_z_min = bb.getMin()[2];
        double cur_z_max = bb.getMax()[2];
        if (cur_z_min <= z_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_z_max >= z_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(0, 0, 1);
        d = Vec3d(0, 0, z_line);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[2] = z_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[2] = z_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }
    
    //z_max
    z_line = objs[i]->getBoundingBox().getMax()[2];
    if (z_line > bounds.getMin()[2] && z_line < bounds.getMax()[2]) {
      double Sa = 0;
      double Sb = 0;
      int Na = 0;
      int Nb = 0;
      std::vector<Geometry*> temp_a_objs;
      std::vector<Geometry*> temp_b_objs;
      for (std::vector<Geometry*>::size_type j = 0; j < objs.size(); j++) {
        BoundingBox bb = objs[j]->getBoundingBox();
        double cur_area = bb.area();
        double cur_z_min = bb.getMin()[2];
        double cur_z_max = bb.getMax()[2];
        if (cur_z_min <= z_line) {
          Sa += cur_area;
          Na += 1;
          temp_a_objs.push_back(objs[j]);
        }
        if (cur_z_max >= z_line) {
          Sb += cur_area;
          Nb += 1;
          temp_b_objs.push_back(objs[j]);
        }
      }
      double C = kt + ki * Na * (Sa / S) + ki * Nb * (Sb / S);
      if (C < bestC) {
        a_objs = temp_a_objs;
        b_objs = temp_b_objs;
        N = Vec3d(0, 0, 1);
        d = Vec3d(0, 0, z_line);
        Vec3d newMaxBounds = bounds.getMax();
        newMaxBounds[2] = z_line;
        Vec3d newMinBounds = bounds.getMin();
        newMinBounds[2] = z_line;
        back = BoundingBox(bounds.getMin(), newMaxBounds);
        front = BoundingBox(newMinBounds, bounds.getMax());
        bestC = C;
      }
    }
  }
  this->front = new kdNode(front);
  this->back = new kdNode(back);
  this->front->fill(b_objs, depth + 1);
  this->back->fill(a_objs, depth + 1);
}

