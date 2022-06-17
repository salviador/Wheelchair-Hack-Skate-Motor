#ifndef DifferentialSteering_h
#define DifferentialSteering_h

#include <stdio.h>


#define COMPUTERANGE 1000.000 //127



void DifferentialSteering_setup(int16_t fPivYLimit);
void DifferentialSteering_computeMotors(int16_t XValue, int16_t YValue);
int16_t DifferentialSteering_computedLeftMotor(void);
int16_t DifferentialSteering_computedRightMotor(void);
int16_t DifferentialSteering_getComputeRange(void);








#endif
