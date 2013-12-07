// LFWindow
// A subclass of FL_GL_Window that handles drawing the traced image to the screen
// 
#include <iostream>
#include <time.h>

#include "LFWindow.h"
#include "../RayTracer.h"
#include "GraphicalUI.h"

#include "../fileio/bitmap.h"

extern bool debugMode;
extern TraceUI* traceUI;

using namespace std;

LFWindow::LFWindow(int x, int y, int w, int h, const char *l)
			: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth = w;
	m_nWindowHeight = h;
	// Do not allow the user to re-size the window
	size_range(w, h, w, h);
}

int LFWindow::handle(int event)
{
  bool s = false;
  if (event == 8) { 
    switch (Fl::event_key()) {
      case 119: // w key
        tracer->moveV(.01);
        s = true;
        break;
      case 97: // a key
        tracer->moveU(-.01);
        s = true;
        break;
      case 115: // s key
        tracer->moveV(-.01);
        s = true;
        break;
      case 100: // d key
        tracer->moveU(.01);
        s = true;
        break;
      case 101: // e key
        tracer->moveLook(.01);
        s = true;
        break;
      case 113: // q key
        tracer->moveLook(-.01);
        s = true;
        break;
      case 120: // x key
        tracer->rotateU(.3);
        s = true;
        break;
      case 122: // z key
        tracer->rotateU(-.3);
        s = true;
        break;
      case 99:
        tracer->rotateV(-.3);
        s = true;
        break;
      case 118:
        tracer->rotateV(.3);
        s = true;
        break;
      default:
        break; 
    }
  } else if(event == FL_PUSH || event == FL_DRAG) {
    s = true;
		int x = Fl::event_x();
		int y = Fl::event_y();
		if(x < 0) x = 0;
		if(x > m_nWindowWidth) x = m_nWindowWidth;
		if(y < 0) y = 0;
		if(y > m_nWindowHeight) y = m_nWindowHeight;

		// Flip for FL's upside-down window coords
		y = m_nWindowHeight - y;

		std::cout << "Clicking " << x << ", " << y << std::endl;
    tracer->tracePixel(x, y);
	}

  if (s) {
	  Fl::flush();
    if (Fl::damage()) {
			Fl::flush();
		}
    refresh();
  }
	return 1;
}

void LFWindow::draw() {
    clock_t start, end;
    start = clock();
	if(!valid()) {
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		ortho();

		m_nWindowWidth=w();
		m_nWindowHeight=h();
	}

	glClear( GL_COLOR_BUFFER_BIT );

	if ( drawbuffer ) {
    updateDrawbuffer();
    // just copy image to GLwindow conceptually
		glRasterPos2i( 0, 0 );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, m_nDrawWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( header.width, header.height, GL_RGB, GL_UNSIGNED_BYTE, drawbuffer );
	}
	glFlush();
  end = clock();
  cout << "drawing took: " << double(end - start) / CLOCKS_PER_SEC << endl;
}

void LFWindow::updateDrawbuffer() {
    // compute bounding box for both planes = 8 ray casts w/ 1 plane intersect
    int max_x, max_y, min_x, min_y;
    //computeRenderRectangle(&max_x, &max_y, &min_x, &min_y);
    // For each pixel in bounding box, cast a lightfield ray
    // if it has u,v,s,t draw a color   = 2 plane intersections
    // else fill it with black
    tracer->traceSetup(header.width, header.height);    
    for (int x = 0; x < header.width; x++) {
	    for (int y = 0; y < header.height; y++) {
        tracer->tracePixel(x, y);   
      }
    }
    tracer->getBuffer(drawbuffer, header.width, header.height);
}

void LFWindow::refresh()
{
  redraw();
}

void LFWindow::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
	// Do not allow manual re-sizing
	size_range(w(), h(), w(), h());
	m_nWindowWidth=w();
	m_nWindowHeight=h();
}


void LFWindow::setBuffer(unsigned char* buf)
{
	buffer = buf;
}

void LFWindow::setHeader(LIGHTFIELD_HEADER* h)
{
	header = *h;
  drawbuffer = new unsigned char [header.width * header.height * 3];
}

void LFWindow::init() {
  tracer = new LFTracer();
  tracer->init(header, buffer);
}
