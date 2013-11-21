#include "curveevaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.00001f;
int CurveEvaluator::s_iSegCount = 16;

float CurveEvaluator::getCutoff() const {
    return cutoff;
}

void CurveEvaluator::setCutoff(float v) {
    cutoff = v;
}

CurveEvaluator::~CurveEvaluator(void)
{
}
