#include "c2.h"
#include <cassert>

using namespace std;

void C2::evaluateCurve(const std::vector<Point>& stupidCtrlPts, 
										 std::vector<Point>& ptvEvaluatedCurvePts, 
										 const float& fAniLength, 
										 const bool& bWrap) const {
	ptvEvaluatedCurvePts.clear();
  std::vector<Point> ptvCtrlPts = std::vector<Point>();
  ptvCtrlPts.assign(stupidCtrlPts.begin(), stupidCtrlPts.end());
  Point fp = ptvCtrlPts[0];
  Point ep = ptvCtrlPts[ptvCtrlPts.size()-1];

  if (!bWrap) {
    /*
    ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
    ptvCtrlPts.insert(ptvCtrlPts.begin(), fp);
    ptvCtrlPts.push_back(ep);
    ptvCtrlPts.push_back(ep);
    */
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
  if (ctrlCount == 0) {
    return;
  }
  if (ctrlCount < 3) {
	  ptvEvaluatedCurvePts.assign(ptvCtrlPts.begin(), ptvCtrlPts.end());
    ptvEvaluatedCurvePts.insert(ptvEvaluatedCurvePts.begin(), Point(0.0, ptvCtrlPts[0].y));
	  ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
    return;
  }
  // compute the d's
  vector<double> gammas = vector<double>();
  gammas.push_back(.5);
  for (int i = 1; i < ctrlCount - 1; i++) {
    gammas.push_back(1/(4.0 - gammas[i - 1]));
  }
  gammas.push_back(1/(2.0 - gammas[ctrlCount - 2]));

  vector<Point> deltas = vector<Point>();
  deltas.push_back(Point(3*gammas[0]*(ptvCtrlPts[1].x-ptvCtrlPts[0].x), 3*gammas[0]*(ptvCtrlPts[1].y-ptvCtrlPts[0].y)));
  for (int i = 1; i < ctrlCount - 1; i++) {
    deltas.push_back(Point(3*gammas[i]*(ptvCtrlPts[i+1].x-ptvCtrlPts[i-1].x) - deltas[i-1].x*gammas[i],
                           3*gammas[i]*(ptvCtrlPts[i+1].y-ptvCtrlPts[i-1].y) - deltas[i-1].y*gammas[i]));
  }
  deltas.push_back(Point(3*gammas[ctrlCount - 1]*(ptvCtrlPts[ctrlCount - 1].x-ptvCtrlPts[ctrlCount - 2].x) - deltas[ctrlCount - 2].x*gammas[ctrlCount - 1],
                         3*gammas[ctrlCount - 1]*(ptvCtrlPts[ctrlCount - 1].y-ptvCtrlPts[ctrlCount - 2].y) - deltas[ctrlCount - 2].y*gammas[ctrlCount - 1]));
  
  vector<Point> Ds = vector<Point>();
  Ds.push_back(deltas[ctrlCount - 1]);
  for (int i = ctrlCount - 2; i >= 0; i--) {
    Ds.insert(Ds.begin(), Point(deltas[i].x - gammas[i]*Ds[0].x, deltas[i].y - gammas[i]*Ds[0].y));
  }

  // add the evaulation points (evaluate the hermite matrix)
  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(Point(0.0, ptvCtrlPts[0].y));
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[0]);
  }
  float x_min = -123132515;
  float delta = .01;
  for (int i = 0; i < ptvCtrlPts.size() - 1; i += 1) {
    Point p0 = ptvCtrlPts[i];
    Point p1 = ptvCtrlPts[i+1];
    Point p2 = Ds[i];
    Point p3 = Ds[i+1];
    for (double j = 0.0; j <= 1.0; j += delta) {
      double j2 = j*j;
      double j3 = j*j2;
      double p0b = 2*j3 - 3*j2 + 1;
      double p1b = -2*j3 + 3*j2;
      double p2b = j3 - 2*j2 + j;
      double p3b = j3 - j2;
      Point f = Point(p0.x*p0b + p1.x*p1b + p2.x*p2b + p3.x*p3b,
                      p0.y*p0b + p1.y*p1b + p2.y*p2b + p3.y*p3b);
      if (f.x > x_min && f.x < ptvCtrlPts[ctrlCount - 1].x) {
        x_min = f.x;
      	ptvEvaluatedCurvePts.push_back(f);
      }
    }
  }


  if (!bWrap) {
    ptvEvaluatedCurvePts.push_back(ptvCtrlPts[ctrlCount-1]);
	  ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[ctrlCount - 1].y));
  }
}
