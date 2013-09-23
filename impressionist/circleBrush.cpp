//
// circleBrush.cpp
//
// The implementation of Circle Brush.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "circleBrush.h"
#include "impressionist.h"
#include <math.h>


extern float frand();

CircleBrush::CircleBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void CircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	BrushMove(source, target);
}

void CircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf( "CircleBrush::BrushMove  document is NULL\n" );
		return;
	}
	float radius = ((float) pDoc->getSize()) / 2;
	SetColor(source, pDoc->getAlpha()); 
  DrawCircle(target, radius);
}

void CircleBrush::DrawCircle(const Point center, const float radius) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2d(center.x, center.y);
  float angle, x, y;
  for (angle = 0; angle < 360; angle += .1) {
    x = center.x + sin(angle * M_PI / 180) * radius;
    y = center.y + cos(angle * M_PI / 180) * radius;
    glVertex2d(x, y);
  }
  glEnd();
}

void CircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

