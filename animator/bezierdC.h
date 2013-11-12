#ifndef INCLUDED_BEZIER_dC_CURVE_EVALUATOR_H
#define INCLUDED_BEZIER_dC_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "curveevaluator.h"

//using namespace std;

class BezierdC : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
		std::vector<Point>& ptvEvaluatedCurvePts, 
		const float& fAniLength, 
		const bool& bWrap) const;
  void helper(Point p0, Point p1, Point p2, Point p3,
		std::vector<Point>* ptvEvaluatedCurvePts) const; 
    
};

#endif
