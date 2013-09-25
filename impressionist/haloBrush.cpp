//
// haloBrush.cpp
//
// The implementation of Halo Brush.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "haloBrush.h"
#include "impressionist.h"
#include <math.h>


extern float frand();

HaloBrush::HaloBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void HaloBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
  pDoc->stopBrush = 0;
	BrushMove(source, target);
}

void HaloBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf( "HaloBrush::BrushMove  document is NULL\n" );
		return;
	}

  if (pDoc->clip && pDoc->getFloatXY(pDoc->gradMag, target.x, target.y) > pDoc->thresh) {
    pDoc->stopBrush = 1;
  }

	float radius = ((float) pDoc->getSize()) / 2;
	SetColor(source, pDoc->getAlpha()); 
  glLineWidth((float)pDoc->getLineThickness());
  DrawHalo(target, radius);
}

void HaloBrush::DrawHalo(const Point center, const float radius) {
  glBegin(GL_LINE_LOOP);
  float angle, x, y;
  for (angle = 0; angle < 361; angle += .1) {
    x = center.x + sin(angle * M_PI / 180) * radius;
    y = center.y + cos(angle * M_PI / 180) * radius;
    glVertex2d(x, y);
  }
  glEnd();
}

void HaloBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

