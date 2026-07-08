#include "main.h"


float prev_err = 0;
float i_value = 0;

int32_t Get_PID_Offset(int32_t target_rpm, float current_rpm, float s_elapsed)
{
    // PID
    float pK = 160;
    float dK = 0;
    float iK = 120;

    float err = target_rpm - current_rpm;

    float d_value = (err - prev_err) / s_elapsed;
    i_value = (i_value + err * s_elapsed);

    prev_err = err;

    int32_t offset = (pK * err) + (dK * d_value) + (iK * i_value);

    return offset;
}

