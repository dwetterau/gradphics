//
// originalview.cpp
//
// The code maintaining the original view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "originalView.h"

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#endif

OriginalView::OriginalView(int			x, 
						   int			y, 
						   int			w, 
						   int			h, 
						   const char*	l)
							: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;

}

void OriginalView::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		// Tell openGL to read from the front buffer when capturing
		// out paint strokes 
		glReadBuffer( GL_FRONT );
		ortho();

	}

	glClear( GL_COLOR_BUFFER_BIT );

	if ( m_pDoc->m_ucBitmap ) 
	{
		// note that both OpenGL pixel storage and the Windows BMP format
		// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
		// around with startrow.

		m_nWindowWidth=w();
		m_nWindowHeight=h();

		int drawWidth, drawHeight;
		GLvoid* bitstart;

		// we are not using a scrollable window, so ignore it
		Point scrollpos;	// = GetScrollPosition();
		scrollpos.x=scrollpos.y=0;

		drawWidth	= min( m_nWindowWidth, m_pDoc->m_nWidth );
		drawHeight	= min( m_nWindowHeight, m_pDoc->m_nHeight );

		int	startrow	= m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
		if ( startrow < 0 ) 
			startrow = 0;


		bitstart = m_pDoc->m_ucBitmap + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

		// just copy image to GLwindow conceptually
		glRasterPos2i( 0, m_nWindowHeight - drawHeight );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart );
	}
	glFlush();
}

static int changedR[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int changedG[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int changedB[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int changedX[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int changedY[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

void OriginalView::drawCross(int x, int y) {
  if (x < 0) {
    x = 0;
  } else if (x >= m_pDoc->m_nWidth) {
    x = m_pDoc->m_nWidth - 1;
  }
  if (y < 0) {
    y = 0;
  } else if (y >= m_pDoc->m_nHeight) {
    y = m_pDoc->m_nHeight - 1;
  }
  int xDiffs[] = {-3,-2,-1,1,2,3,0,0,0,0,0,0};
  int yDiffs[] = {0,0,0,0,0,0,-3,-2,-1,1,2,3};

  int h = m_pDoc->m_nHeight;
  int w = m_pDoc->m_nWidth;

  for (int i = 0; i < 12; i++) {
    int cx = x + xDiffs[i];
    int cy = y + yDiffs[i];
    if (cy < 0 || cy >= h || cx < 0 || cx >= w) {
      changedR[i] = -1;
      changedG[i] = -1;
      changedB[i] = -1;
      changedX[i] = -1;
      changedY[i] = -1;
    } else {
      changedR[i] = m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 0];
      changedG[i] = m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 1];
      changedB[i] = m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 2];
      changedX[i] = cx;
      changedY[i] = cy;
      m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 0] = 255;
      m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 1] = 0;
      m_pDoc->m_ucBitmap[3 * (cy * w + cx) + 2] = 0;
    }
  }
  refresh();
}

void OriginalView::refresh()
{
	redraw();
}

void OriginalView::removeCross() {
  int h = m_pDoc->m_nHeight;
  int w = m_pDoc->m_nWidth;
  // restore them pixels
  for (int i = 0; i < 12; i++) {
    if (changedR[i] != -1) {
      int x = changedX[i];
      int y = changedY[i];
      int R = changedR[i];
      int G = changedG[i];
      int B = changedB[i];
      m_pDoc->m_ucBitmap[3 * (y * w + x) + 0] = (unsigned char) R; 
      m_pDoc->m_ucBitmap[3 * (y * w + x) + 1] = (unsigned char) G; 
      m_pDoc->m_ucBitmap[3 * (y * w + x) + 2] = (unsigned char) B; 
    }
    changedR[i] = -1;
    changedG[i] = -1;
    changedB[i] = -1;
    changedX[i] = -1;
    changedY[i] = -1;
  }
}

void OriginalView::resizeWindow(int	width, 
								int	height)
{
	resize(x(), y(), width, height);
}


