#include "beziercurveevaluator.h"
#include <cassert>

using namespace std;

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const {
	ptvEvaluatedCurvePts.clear();
  int ctrlCount = ptvCtrlPts.size();
  int c = 0;
  double delta = 0.01;
  if (ctrlCount == 0) {
    return;
  }
  if (ctrlCount <  4) {
	  ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0.0, ptvCtrlPts[0].y));
	  ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
    return;
  }
  ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
  ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
  int k = 0;
  for (int i = 0; i < ptvCtrlPts.size() - 3; i += (3 - c)) {
    Point p0 = ptvCtrlPts[i];
    Point p1 = ptvCtrlPts[i + 1];
    Point p2 = ptvCtrlPts[i + 2];
    Point p3 = ptvCtrlPts[i + 3];
    k = i + 3;
    for (double j = delta; j <= 1.0 + delta; j += delta) {
      Point p01 = Point(p0.x * (1 - j) + p1.x * j, p0.y * (1 - j) + p1.y * j);
      Point p12 = Point(p1.x * (1 - j) + p2.x * j, p1.y * (1 - j) + p2.y * j);
      Point p23 = Point(p2.x * (1 - j) + p3.x * j, p2.y * (1 - j) + p3.y * j);
      Point p012 = Point(p01.x * (1 - j) + p12.x * j, p01.y * (1 - j) + p12.y * j);
      Point p123 = Point(p12.x * (1 - j) + p23.x * j, p12.y * (1 - j) + p23.y * j);
      Point f = Point(p012.x * (1 - j) + p123.x * j, p012.y * (1 - j) + p123.y * j);
      ptvEvaluatedCurvePts.push_back(f);
    }
  }
  for (int i = k + 1; i < ptvCtrlPts.size(); i++) {
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);
  }
	ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
}
