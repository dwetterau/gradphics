//
// lineBrush.cpp
//
// The implementation of Line Brush.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "lineBrush.h"
#include "impressionist.h"
#include <math.h>

static int prev_x, prev_y;
extern float frand();

LineBrush::LineBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void LineBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	BrushMove(source, target);
  prev_x = -1; 
  prev_y = -1;
}

void LineBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf( "LineBrush::BrushMove  document is NULL\n" );
		return;
	}
  SetColor(source, pDoc->getAlpha()); 
  //Set size = linewidth
  glLineWidth((float)pDoc->getLineThickness());
  float length = pDoc -> getSize();

	int angleChoice = pDoc->getAngleChoice();
  float angle = 0.0;
  float slider_angle = (float(pDoc->getAngle())) * M_PI / 180.0;
  if (angleChoice == 0) {
    // slider
    angle = slider_angle;  
  } else if (angleChoice == 1) {
    angle = slider_angle; // slider_angle also contains the right click angle 
  } else if (angleChoice == 2) {
    // paint direction
    if (prev_x == -1 || prev_y == -1) {
      angle = slider_angle;
    } else {
      int x_diff = target.x - prev_x;
      int y_diff = target.y - prev_y;
      angle = atan((float)(y_diff) / (float)(x_diff));
    }
    prev_x = target.x;
    prev_y = target.y;
  }
  DrawLine(target, angle, length);
}

void LineBrush::DrawLine(const Point center, const float angle, const float length) {
  glBegin(GL_LINES);
  float x1, y1, x2, y2;
  //angle 1 line
  x1 = (length / 2) * cos(angle) + center.x;
  y1 = (length / 2) * sin(angle) + center.y;
  x2 = (length / 2) * cos(angle + M_PI) + center.x;
  y2 = (length / 2) * sin(angle + M_PI) + center.y;

  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

void LineBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

