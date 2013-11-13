#include "bezierdC.h"
#include <cassert>

using namespace std;

void BezierdC::evaluateCurve(const std::vector<Point>& ctrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const {
	ptvEvaluatedCurvePts.clear();
  vector<Point> ptvCtrlPts = vector<Point>();
  ptvCtrlPts.assign(ctrlPts.begin(), ctrlPts.end());
  if (bWrap) {
    Point a = ptvCtrlPts[0];
    ptvCtrlPts.push_back(Point(a.x + fAniLength, a.y));
  }
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
  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
  }
  ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
  int k = 0;
  for (int i = 0; i < ptvCtrlPts.size() - 3; i += (3 - c)) {
    Point p0 = ptvCtrlPts[i];
    Point p1 = ptvCtrlPts[i + 1];
    Point p2 = ptvCtrlPts[i + 2];
    Point p3 = ptvCtrlPts[i + 3];
    k = i + 3;
    helper(p0, p1, p2, p3, &ptvEvaluatedCurvePts, fAniLength, bWrap);
  }
  for (int i = k; i < ptvCtrlPts.size(); i++) {
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[i]);
  }
  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
  } else {
    if (k + 1 < ptvCtrlPts.size()) {
      Point b = ptvEvaluatedCurvePts[ptvEvaluatedCurvePts.size()-2];
      b = Point(b.x - fAniLength, b.y);
      ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), b);
    }
  }
	//ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
}

void BezierdC::helper(Point p0, Point p1, Point p2, Point p3,
		std::vector<Point>* ptvEvaluatedCurvePts, float fAniLength, bool bWrap) const {
  // check if flat enough
  float dist_segs = p0.distance(p1) + p1.distance(p2) + p2.distance(p3);
  float total_dist = p0.distance(p3);
  if (dist_segs / total_dist < 1 + .00001 || fabs(dist_segs - total_dist) < .0001) {
    ptvEvaluatedCurvePts->push_back(p0);
    if (bWrap) {
      p0 = Point(p0.x - fAniLength, p0.y);
    }
    ptvEvaluatedCurvePts->insert(ptvEvaluatedCurvePts->begin(), p0);
    return;
  }

  Point p01 = Point(p0.x * (.5) + p1.x * .5, p0.y * (.5) + p1.y * .5);
  Point p12 = Point(p1.x * (.5) + p2.x * .5, p1.y * (.5) + p2.y * .5);
  Point p23 = Point(p2.x * (.5) + p3.x * .5, p2.y * (.5) + p3.y * .5);
  Point p012 = Point(p01.x * (.5) + p12.x * .5, p01.y * (.5) + p12.y * .5);
  Point p123 = Point(p12.x * (.5) + p23.x * .5, p12.y * (.5) + p23.y * .5);
  Point f = Point(p012.x * (.5) + p123.x * .5, p012.y * (.5) + p123.y * .5);

  helper(p0, p01, p012, f, ptvEvaluatedCurvePts, fAniLength, bWrap);
  helper(f, p123, p23, p3, ptvEvaluatedCurvePts, fAniLength, bWrap);
}

