#ifndef INCLUDED_CAT_CURVE_EVALUATOR_H
#define INCLUDED_CAT_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  

#include "curveevaluator.h"

//using namespace std;

class Cat : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
		std::vector<Point>& ptvEvaluatedCurvePts, 
		const float& fAniLength, 
		const bool& bWrap) const;
};

#endif
