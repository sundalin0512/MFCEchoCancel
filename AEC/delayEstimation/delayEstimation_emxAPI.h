//
// File: delayEstimation_emxAPI.h
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:26:58
//
#ifndef DELAYESTIMATION_EMXAPI_H
#define DELAYESTIMATION_EMXAPI_H

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
extern FloatArray *emxCreateND_real32_T(int numDimensions, int *size);
extern FloatArray *emxCreateWrapperND_real32_T(float *data, int
  numDimensions, int *size);
extern FloatArray *CreateFloatArray(float *data, int rows, int
  cols);
extern FloatArray *emxCreate_real32_T(int rows, int cols);
extern void emxDestroyArray_real32_T(FloatArray *emxArray);
extern void emxInitArray_real32_T(FloatArray **pEmxArray, int
  numDimensions);

#endif

//
// File trailer for delayEstimation_emxAPI.h
//
// [EOF]
//
