#include "bsplinecurveevaluator.h"
#include <cassert>

using namespace std;

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& stupidCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const {
	ptvEvaluatedCurvePts.clear();
  std::vector<Point> ptvCtrlPts = std::vector<Point>();
  ptvCtrlPts.assign(stupidCtrlPts.begin(), stupidCtrlPts.end());
  Point fp = ptvCtrlPts[0];
  Point ep = ptvCtrlPts[ptvCtrlPts.size()-1];

  ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
  ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
  ptvCtrlPts.push_back(ep);
  ptvCtrlPts.push_back(ep);
  int ctrlCount = ptvCtrlPts.size();
  double delta = 0.01;
  if (ctrlCount == 0) {
    return;
  }
  if (ctrlCount < 4) {
	  ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0.0, ptvCtrlPts[0].y));
	  ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
    return;
  }
  double sixth = 1.0/6.0;
  double third = 2.0/3.0;
  ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
  ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
  int k = 0;
  for (int i = 0; i < ptvCtrlPts.size() - 3; i += 1) {
    Point p0 = ptvCtrlPts[i];
    Point p1 = ptvCtrlPts[i + 1];
    Point p2 = ptvCtrlPts[i + 2];
    Point p3 = ptvCtrlPts[i + 3];
    k = i + 1;
    for (double j = delta; j <= 1.0 + delta; j += delta) {
      double j2 = j*j;
      double j3 = j*j2;
      double p0b = -sixth*j3 + 0.5*j2 - 0.5*j + sixth;
      double p1b = 0.5*j3 - j2 + third;
      double p2b = -0.5*j3 + 0.5*j2 + 0.5*j + sixth;
      double p3b = sixth*j3;
      Point f = Point(p0.x*p0b + p1.x*p1b + p2.x*p2b + p3.x*p3b,
                      p0.y*p0b + p1.y*p1b + p2.y*p2b + p3.y*p3b);
      ptvEvaluatedCurvePts.push_back(f);
    }
  }
  ptvEvaluatedCurvePts.push_back(ptvCtrlPts[ctrlCount-1]);
	ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
}
