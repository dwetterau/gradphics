//
// scatteredLineBrush.cpp
//
// The implementation of Line Brush.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "scatteredLineBrush.h"
#include "impressionist.h"
#include <math.h>


static int prev_x, prev_y;
extern float frand();

ScatteredLineBrush::ScatteredLineBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredLineBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	BrushMove(source, target);
}

void ScatteredLineBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if (pDoc == NULL) {
		printf( "ScatteredLineBrush::BrushMove  document is NULL\n" );
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
  } else if (angleChoice == 3) {
    angle = pDoc->gradDir[target.y * pDoc->m_nWidth + target.x];
    angle += M_PI / 2.0;
    if (angle > 2 * M_PI) {
      angle -= 2 * M_PI;
    }
  }

  float random_angle = frand() * M_PI * 2; 
  float x_diff = (length / 2) * cos(random_angle);
  float y_diff = (length / 2) * sin(random_angle);

  Point* positive_point = new Point(target.x + x_diff, target.y + y_diff);
  Point* negative_point = new Point(target.x - x_diff, target.y - y_diff);
  DrawLine(target, angle, length);
  DrawLine(*positive_point, angle, length);
  DrawLine(*negative_point, angle, length);
}

void ScatteredLineBrush::DrawLine(const Point center, const float angle, const float length) {
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

void ScatteredLineBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

