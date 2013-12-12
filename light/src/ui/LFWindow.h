#ifndef __LF_WINDOW_H__
#define __LF_WINDOW_H__

#include <FL/Fl.H>

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <vector>

#include "../RayTracer.h"
#include "../LFTracer.h"
#include "../fileio/bitmap.h"

class LFWindow : public Fl_Gl_Window
{
public:
	LFWindow(int x, int y, int w, int h, const char *l);
	void draw();
	int handle(int event);

	void refresh();

	void resizeWindow(int width, int height);
    void updateDrawbuffer();

	void setHeader(LIGHTFIELD_HEADER *h);
	void setBuffer(unsigned char * buf);
  void init();
	void addHeader(LIGHTFIELD_HEADER *h);
	void addBuffer(unsigned char * buf);

private:
  unsigned char* drawbuffer;
	  unsigned char* buffer;
    LIGHTFIELD_HEADER header;

    std::vector<unsigned char*> buffers;
    std::vector<LIGHTFIELD_HEADER> headers;
	int m_nWindowWidth, m_nWindowHeight;
  int m_nDrawWidth, m_nDrawHeight;
  LFTracer* tracer;
  int prevx;
  int prevy;
  int iters;
  double timing;
};

#endif // __LF_WINDOW_H__

