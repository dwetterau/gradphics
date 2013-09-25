//
// haloBrush.h
//
// The header file for Halo Brush. 
//

#ifndef HALOBRUSH_H
#define HALOBRUSH_H

#include "impBrush.h"

class HaloBrush : public ImpBrush
{
public:
	HaloBrush( ImpressionistDoc* pDoc = NULL, const char* name = NULL );

	void BrushBegin( const Point source, const Point target );
	void BrushMove( const Point source, const Point target );
	void BrushEnd( const Point source, const Point target );
	void DrawHalo(const Point center, const float radius);
  char* BrushName( void );
};

#endif
