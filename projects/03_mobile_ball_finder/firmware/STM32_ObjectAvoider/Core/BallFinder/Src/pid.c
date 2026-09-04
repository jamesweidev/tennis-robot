#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <math.h>
#include <stdio.h>


#define LEFT_PK 0.0016f
#define RIGHT_PK 0.0015f
#define LEFT_IK 0.0043f
#define RIGHT_IK 0.0035f

extern Encoder right_encoder;

float Get_PID_Correction(Encoder* enc)
{
    float pK = (enc == &right_encoder) ? RIGHT_PK : LEFT_PK;
    float iK = (enc == &right_encoder) ? RIGHT_IK : LEFT_IK;

    float err = enc->target_rpm - enc->current_rpm;

    // Prevents integral from accumulating while within the deathband
    float rpm_per_tick = 60 / (TICKS_PER_ROTATION * enc->s_elapsed); // Deathband range
    if (fabsf(err) > (rpm_per_tick))
    {
        enc->pid.i_value = (enc->pid.i_value + err * enc->s_elapsed);
    }

    // Cap the integral to 200. prevent massive sudden bursts
    if (enc->pid.i_value > 220) enc->pid.i_value = 220;
    if (enc->pid.i_value < -220) enc->pid.i_value = -220;


    enc->pid.prev_err = err;

    float offset = (pK * err) + (iK * enc->pid.i_value);

    return offset;
}
