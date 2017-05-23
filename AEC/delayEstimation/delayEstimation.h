//
// File: delayEstimation.h
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:26:58
//
#ifndef DELAYESTIMATION_H
#define DELAYESTIMATION_H

// Include Files
#include <cmath>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "delayEstimation_types.h"

// Function Declarations
extern void LMS(const FloatArray *xn, const FloatArray *dn, float
                M, float mu, float itr, FloatArray *b_yn,
                FloatArray *W, FloatArray *en);
extern void NLMS(const FloatArray *xn, const FloatArray *dn, float
                 M, float mu, float itr, FloatArray *b_yn,
                 FloatArray *W, FloatArray *en);
extern void RLS(const FloatArray *xn, const FloatArray *dn, float
                M, float itr, FloatArray *b_yn, FloatArray *W,
                FloatArray *en);
extern float delayEstimation(const FloatArray *farEndSound, const
  FloatArray *nearEndSound);
extern void delayEstimation_initialize();
extern void delayEstimation_terminate();

#endif

//
// File trailer for delayEstimation.h
//
// [EOF]
//
