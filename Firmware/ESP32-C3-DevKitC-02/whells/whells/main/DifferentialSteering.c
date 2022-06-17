#include "DifferentialSteering.h"

int16_t m_fPivYLimit;
int16_t m_leftMotor;
int16_t m_rightMotor;


/**
 * Initialize the threshold at which the pivot action starts.
 *
 * @param fPivYLimit: Threshold. Is measured in units on the Y-axis away from the X-axis (Y=0).
 *                    A greater value will assign more of the joystick's range to pivot actions.
 *                    Allowable range: (0..+127).
 */
void DifferentialSteering_setup(int16_t fPivYLimit){
    m_leftMotor = 0;
    m_rightMotor = 0;
    m_fPivYLimit = fPivYLimit;
}

/**
 * Compute differential steering from (x,y) values.
 *
 * @param XValue: X value in [-127, 127] range.
 * @param YValue: Y value in [-127, 127] range.
 */
void DifferentialSteering_computeMotors(int16_t XValue, int16_t YValue){
    float   nMotPremixL = 0;    // Motor (left)  premixed output        (-127..+127)
    float   nMotPremixR = 0;    // Motor (right) premixed output        (-127..+127)
    int16_t     nPivSpeed = 0;      // Pivot Speed                          (-127..+127)
    float   fPivScale = 0;      // Balance scale b/w drive and pivot    (   0..1   )

    // Calculate Drive Turn output due to Joystick X input
    if (YValue >= 0) {
        // Forward
        nMotPremixL = (XValue >= 0) ? COMPUTERANGE : (COMPUTERANGE + (float)XValue);
        nMotPremixR = (XValue >= 0) ? (COMPUTERANGE - (float)XValue) : COMPUTERANGE;
    } else {
        // Reverse
        nMotPremixL = (XValue >= 0) ? (COMPUTERANGE - (float)XValue) : COMPUTERANGE;
        nMotPremixR = (XValue >= 0) ? COMPUTERANGE : (COMPUTERANGE + (float)XValue);
    }

    // Scale Drive output due to Joystick Y input (throttle)
    nMotPremixL = nMotPremixL * (float)YValue / COMPUTERANGE;
    nMotPremixR = nMotPremixR * (float)YValue / COMPUTERANGE;

    // Now calculate pivot amount
    // - Strength of pivot (nPivSpeed) based on Joystick X input
    // - Blending of pivot vs drive (fPivScale) based on Joystick Y input
    nPivSpeed = XValue;
    
    //ABS
    if(YValue < 0){
        YValue = -YValue;
    }

    //fPivScale = (ABS(YValue) > m_fPivYLimit) ? 0.0 : (1.0 - ABS(YValue) / m_fPivYLimit);
    fPivScale =(float) ( (YValue > m_fPivYLimit) ? 0.0 : (1.0 - YValue / m_fPivYLimit) );

    // Calculate final mix of Drive and Pivot
    m_leftMotor  = (1.0 - fPivScale) * nMotPremixL + fPivScale * ( nPivSpeed);
    m_rightMotor = (1.0 - fPivScale) * nMotPremixR + fPivScale * (-nPivSpeed);
}

/*
 * Returns the value of the left motor computed in computeMotors method.
 *
 * @return left computed motor, in [-127, 127] range.
 */
int16_t DifferentialSteering_computedLeftMotor(void) {
    return m_leftMotor;
}

/*
 * Returns the value of the right motor computed in computeMotors method.
 *
 * @return right computed motor, in [-127, 127] range.
 */
int16_t DifferentialSteering_computedRightMotor(void) {
    return m_rightMotor;
}

/**
 * Return the compute range used in the computeMotors method.
 * 
 * @return the compute range.
 */
int16_t DifferentialSteering_getComputeRange(void) {
    return COMPUTERANGE;
}
