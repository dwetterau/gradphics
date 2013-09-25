//
// scatteredCircleBrush.cpp
//
// The implementation of Scattered Circle Brush.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "scatteredCircleBrush.h"
#include "impressionist.h"
#include <math.h>


extern float frand();

ScatteredCircleBrush::ScatteredCircleBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredCircleBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
  pDoc->stopBrush = 0;
	BrushMove(source, target);
}

void ScatteredCircleBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf( "ScatteredCircleBrush::BrushMove  document is NULL\n" );
		return;
	}
  if (pDoc->clip && pDoc->getFloatXY(pDoc->gradMag, target.x, target.y) > pDoc->thresh) {
    pDoc->stopBrush = 1;
  }

	float radius = ((float) pDoc->getSize()) / 2;
  float radius_factor = .4; // The radius of the scattered circle
  SetColor(source, pDoc->getAlpha()); 
  // determine random new center
  // first find the random angle
  float random_angle = frand() * M_PI * 2;
  int x = target.x + radius * (1 - radius_factor) * sin(random_angle);
  int y = target.y + radius * (1 - radius_factor) * cos(random_angle);
  Point* small_circle_center = new Point(x, y);
  DrawCircle(*small_circle_center, radius_factor * radius);
}

void ScatteredCircleBrush::DrawCircle(const Point center, const float radius) {
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

void ScatteredCircleBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

