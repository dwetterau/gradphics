// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//


#include <FL/fl_ask.H>
#include <math.h>
#include <algorithm>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "impBrush.h"

// Include individual brush headers here.
#include "pointBrush.h"
#include "circleBrush.h"
#include "scatteredPointBrush.h"
#include "scatteredCircleBrush.h"
#include "lineBrush.h"
#include "scatteredLineBrush.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

ImpressionistDoc::ImpressionistDoc() 
{
	// Set NULL image name as init. 
	m_imageName[0]	='\0';	

	m_nWidth		= -1;
	m_ucBitmap		= NULL;
	m_ucPainting	= NULL;
	m_ucPreviewBackup = NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]				
		= new LineBrush( this, "Lines" );
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]				
		= new CircleBrush( this, "Circles" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]	
		= new ScatteredPointBrush( this, "Scattered Points" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]		
		= new ScatteredLineBrush( this, "Scattered Lines" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]	
		= new ScatteredCircleBrush( this, "Scattered Circles" );

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];
  clip = 0;
  stopBrush = 0;
  thresh = 50.0;
}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

void ImpressionistDoc::setAngleChoice(int type)
{
  m_angleChoice	= type;
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

float ImpressionistDoc::getAlpha() {
  return m_pUI->getAlpha();
}

int ImpressionistDoc::getLineThickness() {
  return m_pUI->getLineThickness();
}

int ImpressionistDoc::getAngle() {
  return m_pUI->getAngle();
}

void ImpressionistDoc::setAngle(int angle) {
  m_pUI->setAngle(angle);
}

int ImpressionistDoc::getAngleChoice() {
  return m_angleChoice;
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(iname, width, height))==NULL ) 
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth		= width;
	m_nPaintWidth	= width;
	m_nHeight		= height;
	m_nPaintHeight	= height;

	// release old storage
	delete [] m_ucBitmap;
	delete [] m_ucPainting;
	delete [] m_ucPreviewBackup;

	m_ucBitmap		= data;

  gradMag = new float [width*height];
  gradDir = new float [width*height];

  fillGradBuffers(m_ucBitmap, width, height, gradMag, gradDir);

	// allocate space for draw view
	m_ucPainting		= new unsigned char [width*height*3];
	m_ucPreviewBackup	= new unsigned char [width*height*3];
	memset(m_ucPainting, 0, width*height*3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(), 
								m_pUI->m_mainWindow->y(), 
								width*2, 
								height+25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);	
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);	
	m_pUI->m_paintView->refresh();


	return 1;
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname) 
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas() 
{

	// Release old storage
	if ( m_ucPainting ) 
	{
		delete [] m_ucPainting;

		// allocate space for draw view
		m_ucPainting	= new unsigned char [m_nPaintWidth*m_nPaintHeight*3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}
	
	return 0;
}

// Apply the filter specified by filter_kernel to the 
// each pixel in the source buffer and place the resulting
// pixel in the destination buffer.  

// This is called from the UI when the
// "preview" or "apply" button is pressed in the filter dialog.



/*
 *	INPUT: 
 *		sourceBuffer:		the original image buffer, 
 *		srcBufferWidth:		the width of the image buffer
 *		srcBufferHeight:	the height of the image buffer
 *							the buffer is arranged such that 
 *							origImg[3*(row*srcBufferWidth+column)+0], 
 *							origImg[3*(row*srcBufferWidth+column)+1], 
 *							origImg[3*(row*srcBufferWidth+column)+2]
 *							are R, G, B values for pixel at (column, row).
 *		destBuffer:			the image buffer to put the resulting
 *							image in.  It is always the same size
 *							as the source buffer.
 *
 *      filterKernel:		the 2D filter kernel,
 *		knlWidth:			the width of the kernel
 *		knlHeight:			the height of the kernel
 *
 *		divisor, offset:	each pixel after filtering should be
 *							divided by divisor and then added by offset
 */
void ImpressionistDoc::applyFilter( const unsigned char* sourceBuffer,
		int srcBufferWidth, int srcBufferHeight,
		unsigned char* destBuffer,
		int **filterKernel, 
		int knlWidth, int knlHeight, 
		double divisor, double offset )
{
	// This needs to be implemented for image filtering to work.
  for (int r = 0; r < srcBufferHeight; r++) {
    for (int c = 0; c < srcBufferWidth; c++) {
      int r_total_sum = 0;
      int g_total_sum = 0;
      int b_total_sum = 0;
      int r_offset = -knlHeight / 2;
      int c_offset = -knlWidth / 2;
      for (int kr = 0; kr < knlHeight; kr++) {
        for (int kc = 0; kc < knlWidth; kc++) {
          r_total_sum += filterKernel[kr][kc] * int(getColor(sourceBuffer, r + r_offset + kr, c + c_offset + kc, srcBufferWidth, srcBufferHeight, 0));
          g_total_sum += filterKernel[kr][kc] * int(getColor(sourceBuffer, r + r_offset + kr, c + c_offset + kc, srcBufferWidth, srcBufferHeight, 1));
          b_total_sum += filterKernel[kr][kc] * int(getColor(sourceBuffer, r + r_offset + kr, c + c_offset + kc, srcBufferWidth, srcBufferHeight, 2));
        }
      }
      r_total_sum = bound(int(r_total_sum / divisor + offset));
      g_total_sum = bound(int(g_total_sum / divisor + offset));
      b_total_sum = bound(int(b_total_sum / divisor + offset));
      destBuffer[3 * (r * srcBufferWidth + c) + 0] = r_total_sum;
      destBuffer[3 * (r * srcBufferWidth + c) + 1] = g_total_sum;
      destBuffer[3 * (r * srcBufferWidth + c) + 2] = b_total_sum;
    }
  }
}

unsigned char ImpressionistDoc::bound(int input) {
  unsigned char response;
  if (input > 255) {
    response = 255;
  } else if (input < 0) {
    response = 0;
  } else {
    response = input;
  }
  return response;
}

unsigned char ImpressionistDoc::getColor(const unsigned char* buf, int r, int c, int width, int height, int offset) {
  if (r < 0) {
    r = 0;
  } else if (r >= height) {
    r = height - 1;
  }
  if (c < 0) {
    c = 0;
  } else if (c >= width) {
    c = width - 1;
  }
  return buf[3 * (r * width + c) + offset];
}

float ImpressionistDoc::getFloatXY(float* buf, int x, int y) {
  int width = m_nWidth;
  int height = m_nHeight;
  if (y < 0) {
    y = 0;
  } else if (y >= height) {
    y = height - 1;
  }
  if (x < 0) {
    x = 0;
  } else if (x >= width) {
    x = width - 1;
  }
  return buf[x + y * width];
}


//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( int x, int y )
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= m_nWidth ) 
		x = m_nWidth-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= m_nHeight ) 
		y = m_nHeight-1;

	return (GLubyte*)(m_ucBitmap + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( const Point p )
{
	return GetOriginalPixel( p.x, p.y );
}


void ImpressionistDoc::fillGradBuffers(
    const unsigned char* src, int w, int h, float* magBuf, float* dirBuf) {
  unsigned char* smoothed = new unsigned char [w * h * 3];
  int ks = 5;
  int ** kernel = new int* [ks];
  for (int r = 0; r < ks; r++) {
    kernel[r] = new int [ks];
    for (int c = 0; c < ks; c++) {
     kernel[r][c] = 1;
    }
  }
  applyFilter(src, w, h, smoothed, kernel, ks, ks, float(ks * ks), 0.0); 
  const unsigned char* const_smoothed = (const unsigned char*) smoothed;
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      float xdiff = 0, ydiff = 0;
      for (int o = 0; o < 3; o++) {
        float dx = getColor(const_smoothed, y, x + 1, w, h, o) - getColor(const_smoothed, y, x - 1, w, h, o);
        float dy = getColor(const_smoothed, y + 1, x, w, h, o) - getColor(const_smoothed, y - 1, x, w, h, o);
        xdiff += dx;
        ydiff += dy;
      }
      xdiff /= 3.0;
      ydiff /= 3.0;
      magBuf[y * w + x] = sqrt(xdiff*xdiff + ydiff*ydiff);
      float radians = atan(ydiff / xdiff);
      if (radians > M_PI * 2) {
        radians = M_PI * 2;
      } else if (radians < -M_PI * 2) {
        radians = -M_PI * 2;
      }
      dirBuf[y * w + x] = radians;
    }
  }
}



