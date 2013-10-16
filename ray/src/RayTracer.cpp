// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
Vec3d RayTracer::trace( double x, double y )
{
	// Clear out the ray cache in the scene for debugging purposes,
	scene->intersectCache.clear();

    ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY );

    scene->getCamera().rayThrough( x,y,r );
	Vec3d ret = traceRay( r, Vec3d(1.0,1.0,1.0), 0 );
	ret.clamp();
	return ret;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
Vec3d RayTracer::traceRay( const ray& r, const Vec3d& thresh, int depth )
{
  int limit = traceUI->getDepth();
  if (depth > limit) {
    return Vec3d(0,0,0);
  }
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
    
    Vec3d N = i.N;
    Vec3d D = r.getDirection();
    Vec3d V = -D; 
    double ni, nt;
    Vec3d Pr, Pt;
    if (D * N < 0) {
      // going in
      ni = 1.0;
      nt = m.index(i);
    } else {
      N = -N;
      // going out
      ni = m.index(i);
      nt = 1.0;
    }
    Vec3d Ci = (V * N) * N;
    Vec3d Si = Ci + D;
    Vec3d R = Ci + Si;
    Pr = r.at(i.t - RAY_EPSILON);
    Pt = r.at(i.t + RAY_EPSILON);
    
    Vec3d St = (ni / nt) * Si;
    Vec3d Ct = pow(1 - St*St, 0.5) * (-N);
    Vec3d T = St + Ct;
    Vec3d local = m.shade(scene, r, i);
   
    Vec3d kr = m.kr(i);
    Vec3d kt = m.kt(i);
    Vec3d ref, tra;

    if (kr[0] > 0 || kr[1] > 0 || kr[2] > 0) {
      ref = traceRay(ray(Pr, R, ray::REFLECTION), thresh, depth + 1);
    } else {
      ref = Vec3d(0,0,0);
    }
    if (T[0] == T[0] && (kt[0] > 0 || kt[1] > 0 || kt[2] > 0)) {
      tra = traceRay(ray(Pt, T, ray::REFRACTION), thresh, depth + 1);
	  } else {
      tra = Vec3d(0,0,0);
    }

    ref[0] *= kr[0];
    ref[1] *= kr[1];
    ref[2] *= kr[2];

    tra[0] *= kt[0];
    tra[1] *= kt[1];
    tra[2] *= kt[2];

    local[0] += ref[0];
    local[1] += ref[1];
    local[2] += ref[2];

    local[0] += tra[0];
    local[1] += tra[1];
    local[2] += tra[2];
    
    return local;
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
    if (scene->cubeMap()) {
      double u, v;
      Vec3d xp = Vec3d(1,0,0);
      Vec3d xn = Vec3d(-1,0,0);
      Vec3d yp = Vec3d(0,1,0);
      Vec3d yn = Vec3d(0,-1,0);
      Vec3d zp = Vec3d(0,0,1);
      Vec3d zn = Vec3d(0,0,-1);
      double sqrt2 = pow(2, .5);
      //double best = 1 / sqrt2 + .1;
      Vec3d ray = r.getDirection();
      int map = 0;
      double biggest = 0;
      if (abs(ray[0]) > biggest) {
        biggest = abs(ray[0]);
        if (ray[0] > 0) {
          map = 0;
        } else {
          map = 1;
        }
      }
      if (abs(ray[1]) > biggest) {
        biggest = abs(ray[1]);
        if (ray[1] > 0) {
          map = 2;
        } else {
          map = 3;
        }
      }
      if (abs(ray[2]) > biggest) {
        biggest = abs(ray[2]);
        if (ray[2] > 0) {
          map = 4;
        } else {
          map = 5;
        }
      }
      switch(map) {
        case 0:
          u = ray * zp;
          v = ray * yp;
          break;
        case 1:
          u = ray * zn;
          v = ray * yp;
          break;
        case 2:
          u = ray * xp;
          v = ray * zp;
          break;
        case 3:
          u = ray * xp;
          v = ray * zn;
          break;
        case 4:
          u = ray * xn;
          v = ray * yp;
          break;
        case 5:
          u = ray * xp;
          v = ray * yp;
          break;
        default:
          cout << "unknown map?" << endl;
          break;
      }
      u = (sqrt2 * u + 1.0) / 2.0;
      v = (sqrt2 * v + 1.0) / 2.0;
      switch(map) {
        case 0:
          return scene->xpMap->getMappedValue(Vec2d(u, v));
          break;
        case 1:
          return scene->xnMap->getMappedValue(Vec2d(u, v));
          break;
        case 2:
          return scene->ypMap->getMappedValue(Vec2d(u, v));
          break;
        case 3:
          return scene->ynMap->getMappedValue(Vec2d(u, v));
          break;
        case 4:
          return scene->zpMap->getMappedValue(Vec2d(u, v));
          break;
        case 5:
          return scene->znMap->getMappedValue(Vec2d(u, v));
          break;
        default:
          cout << "unknow map??" << endl;
          break;
      }
    } else {
		  return Vec3d( 0.0, 0.0, 0.0 );
	  }
  }
}

RayTracer::RayTracer()
	: scene( 0 ), buffer( 0 ), buffer_width( 256 ), buffer_height( 256 ), m_bBufferReady( false )
{

}


RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn )
{
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos )
		path = ".";
	else
		path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( ! sceneLoaded() ) { 
    return false;
  }
  reloadScene();
  return true;
}

void RayTracer::reloadScene() {
  scene->buildKdTree();
  scene->loadCubeMap();
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];


	}
	memset( buffer, 0, w*h*3 );
	m_bBufferReady = true;
}

void RayTracer::tracePixel( int i, int j )
{
	Vec3d col = Vec3d(0,0,0);
  double aa = traceUI->getAa();
  if( ! sceneLoaded() )
		return;
  double div = aa * aa;

  if (traceUI->getStoc()) {
    for (int r = 0; r < aa * aa; r++) {
      double x = double(i + double(rand())/double(RAND_MAX)) / double(buffer_width);
      double y = double(j + double(rand())/double(RAND_MAX)) / double(buffer_width);
    
      Vec3d tempCol = trace(x, y);

      col[0] += tempCol[0] / div;
      col[1] += tempCol[1] / div;
      col[2] += tempCol[2] / div;
    }
  } else {
    for (int r = 0; r < aa; r++) {
      for (int c = 0; c < aa; c++) {
        double x = double(i + r/aa)/double(buffer_width);
	      double y = double(j + c/aa)/double(buffer_height);
  
	      Vec3d tempCol = trace(x, y);

        col[0] += tempCol[0] / div;
        col[1] += tempCol[1] / div;
        col[2] += tempCol[2] / div;
      }
    }
  }
  unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}


