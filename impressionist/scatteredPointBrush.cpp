//
// scatteredPointBrush.cpp
//
// The implementation of Scattered Point Brush. 
// 

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "scatteredPointBrush.h"

extern float frand();

ScatteredPointBrush::ScatteredPointBrush( ImpressionistDoc* pDoc, const char* name ) :
	ImpBrush(pDoc,name)
{
}

void ScatteredPointBrush::BrushBegin( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;
	glPointSize( 1 );

	BrushMove( source, target );
}

void ScatteredPointBrush::BrushMove( const Point source, const Point target )
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg=pDoc->m_pUI;

	if ( pDoc == NULL ) {
		printf( "ScatteredPointBrush::BrushMove  document is NULL\n" );
		return;
	}
	int size = pDoc->getSize();
  SetColor(source, pDoc->getAlpha());  

	glBegin( GL_POINTS );
  int x,y;
  float random_cutoff = .1;
  for (x = -size / 2; x < size / 2; x++) {
    for (y= -size / 2; y < size / 2; y++) {
      if (frand() < random_cutoff) {
        glVertex2d(target.x + x, target.y + y);
      }
    }
  }
	glEnd();
}

void ScatteredPointBrush::BrushEnd( const Point source, const Point target )
{
	// do nothing so far
}

