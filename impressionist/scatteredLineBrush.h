//
// scatteredLineBrush.h
//
// The header file for Line Brush. 
//

#ifndef SCATTEREDLINEBRUSH_H
#define SCATTEREDLINEBRUSH_H

#include "impBrush.h"

class ScatteredLineBrush : public ImpBrush
{
public:
	ScatteredLineBrush( ImpressionistDoc* pDoc = NULL, const char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	void DrawLine(const Point center, const float angle, const float length);
  char* BrushName( void );
};

#endif
