#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include <math.h>
#include <stdio.h>

float Get_PID_Correction(Encoder* enc)
{
    // PID
    float pK = 0.0015f;
    float iK = 0.0035f;
    float dK = 0.00f;

    float err = enc->target_rpm - enc->current_rpm;

    float d_value = (err - enc->pid.prev_err) / S_ELAPSED;

    // Prevents integral from accumulating while within the deathband
    float rpm_per_tick = 60 / (TICKS_PER_ROTATION * GEAR_RATIO * S_ELAPSED); // Deathband range
    if (fabsf(err) > (rpm_per_tick + 1))
    {
        enc->pid.i_value = (enc->pid.i_value + err * S_ELAPSED);
    }

    // Cap the integral to 100
    // if (enc->pid.i_value > 100) enc->pid.i_value = 100;
    // if (enc->pid.i_value < -100) enc->pid.i_value = -100;


    enc->pid.prev_err = err;

    float offset = (pK * err) + (dK * d_value) + (iK * enc->pid.i_value);

    return offset;
}
