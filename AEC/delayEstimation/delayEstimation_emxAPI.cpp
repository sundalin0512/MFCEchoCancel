//
// File: delayEstimation_emxAPI.cpp
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:26:58
//

// Include Files
#include "rt_nonfinite.h"
#include "delayEstimation.h"
#include "delayEstimation_emxAPI.h"
#include "delayEstimation_emxutil.h"

// Function Definitions

//
// Arguments    : int numDimensions
//                int *size
// Return Type  : emxArray_real32_T *
//
FloatArray *emxCreateND_real32_T(int numDimensions, int *size)
{
  FloatArray *emx;
  int numEl;
  int i;
  emxInit_real32_T(&emx, numDimensions);
  numEl = 1;
  for (i = 0; i < numDimensions; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (float *)calloc((unsigned int)numEl, sizeof(float));
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  return emx;
}

//
// Arguments    : float *data
//                int numDimensions
//                int *size
// Return Type  : emxArray_real32_T *
//
FloatArray *emxCreateWrapperND_real32_T(float *data, int numDimensions,
  int *size)
{
  FloatArray *emx;
  int numEl;
  int i;
  emxInit_real32_T(&emx, numDimensions);
  numEl = 1;
  for (i = 0; i < numDimensions; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = numDimensions;
  emx->allocatedSize = numEl;
  emx->canFreeData = false;
  return emx;
}

//
// Arguments    : float *data
//                int rows
//                int cols
// Return Type  : emxArray_real32_T *
//
FloatArray *CreateFloatArray(float *data, int rows, int cols)
{
  FloatArray *emx;
  int size[2];
  int numEl;
  int i;
  size[0] = rows;
  size[1] = cols;
  emxInit_real32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = data;
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  emx->canFreeData = false;
  return emx;
}

//
// Arguments    : int rows
//                int cols
// Return Type  : emxArray_real32_T *
//
FloatArray *emxCreate_real32_T(int rows, int cols)
{
  FloatArray *emx;
  int size[2];
  int numEl;
  int i;
  size[0] = rows;
  size[1] = cols;
  emxInit_real32_T(&emx, 2);
  numEl = 1;
  for (i = 0; i < 2; i++) {
    numEl *= size[i];
    emx->size[i] = size[i];
  }

  emx->data = (float *)calloc((unsigned int)numEl, sizeof(float));
  emx->numDimensions = 2;
  emx->allocatedSize = numEl;
  return emx;
}

//
// Arguments    : emxArray_real32_T *emxArray
// Return Type  : void
//
void emxDestroyArray_real32_T(FloatArray *emxArray)
{
  emxFree_real32_T(&emxArray);
}

//
// Arguments    : emxArray_real32_T **pEmxArray
//                int numDimensions
// Return Type  : void
//
void emxInitArray_real32_T(FloatArray **pEmxArray, int numDimensions)
{
  emxInit_real32_T1(pEmxArray, numDimensions);
}

//
// File trailer for delayEstimation_emxAPI.cpp
//
// [EOF]
//
