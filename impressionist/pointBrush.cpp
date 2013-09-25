//
// pointBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "pointBrush.h"

extern float frand();

PointBrush::PointBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void PointBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
  pDoc->stopBrush = 0;
	int size = pDoc->getSize();

	glPointSize( (float)size );
  
	BrushMove( source, target );
}

void PointBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	if ( pDoc == NULL ) {
		printf( "PointBrush::BrushMove  document is NULL\n" );
		return;
	}
  printf("%f\n", pDoc->thresh);
  if (pDoc->clip && pDoc->getFloatXY(pDoc->gradMag, target.x, target.y) > pDoc->thresh) {
    pDoc->stopBrush = 1;
  }
	SetColor(source, pDoc->getAlpha());
	glBegin( GL_POINTS );

	glVertex2d( target.x, target.y );

	glEnd();
}

void PointBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

