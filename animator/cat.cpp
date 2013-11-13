#include "cat.h"
#include <cassert>

using namespace std;

void Cat::evaluateCurve(const std::vector<Point>& stupidCtrlPts,
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const {
	ptvEvaluatedCurvePts.clear();
  std::vector<Point> ptvCtrlPts = std::vector<Point>();
  ptvCtrlPts.assign(stupidCtrlPts.begin(), stupidCtrlPts.end());
  Point fp = ptvCtrlPts[0];
  Point ep = ptvCtrlPts[ptvCtrlPts.size()-1];

  if (!bWrap) {
    ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
    ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
    ptvCtrlPts.push_back(ep);
    ptvCtrlPts.push_back(ep);
  } else {
    // push on some new front points mannn
    std::vector<Point> newFrontPts = std::vector<Point>();
    int i = ptvCtrlPts.size() - 1;
    int j = 1;
    while(newFrontPts.size() < 3) {
      if (i < 0) {
        j++;
        i += ptvCtrlPts.size();
      }
      newFrontPts.push_back(Point(ptvCtrlPts[i].x - j * fAniLength, ptvCtrlPts[i].y));
      i--;
    }
    // push on some new back points
    std::vector<Point> newBackPts = std::vector<Point>();
    i = 0;
    j = 1;
    while(newBackPts.size() < 3) {
      if (i >= ptvCtrlPts.size()) {
        j++;
        i -= ptvCtrlPts.size();
      }
      newBackPts.push_back(Point(ptvCtrlPts[i].x + j * fAniLength, ptvCtrlPts[i].y));
      i++;
    }
    for (i = 0; i < newFrontPts.size(); i++) {
      ptvCtrlPts.insert(ptvCtrlPts.begin(), newFrontPts[i]);
    }
    for (i = 0; i < newBackPts.size(); i++) {
      ptvCtrlPts.push_back(newBackPts[i]);
    }
  }
  
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
  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
  }
  int k = 0;
  float x_lim = -1231241243;
  for (int i = 1; i < ptvCtrlPts.size() - 1; i += 1) {
    Point p0 = ptvCtrlPts[i - 1];
    Point p1 = ptvCtrlPts[i];
    Point p2 = ptvCtrlPts[i + 1];
    Point p3 = ptvCtrlPts[i + 2];
    k = i + 1;
    for (double j = delta; j <= 1.0 + delta; j += delta) {
      double j2 = j*j;
      double j3 = j*j2;
      double p0b = -.5*j3 + j2 - .5*j;
      double p1b = 1.5*j3 - 2.5*j2 + 1;
      double p2b = -1.5*j3 + 2*j2 + 0.5*j;
      double p3b = .5*j3 - .5*j2;
      Point f = Point(p0.x*p0b + p1.x*p1b + p2.x*p2b + p3.x*p3b,
                      p0.y*p0b + p1.y*p1b + p2.y*p2b + p3.y*p3b);
      if (f.x > x_lim && f.x < ptvCtrlPts[ctrlCount-1].x) {
        x_lim = f.x;
        ptvEvaluatedCurvePts.push_back(f);
      }
    }
  }
  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[ctrlCount-1]);
    ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
  }
}
