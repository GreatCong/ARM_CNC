#ifndef __ARM_MOTION_H_
#define __ARM_MOTION_H_

#include "util.h"

#define ARM //¶¨Òå»úÐµ±Û

#ifdef ARM
typedef struct{
  float arm[N_AXIS];
	uint8_t transfer_state;
}ARM_Motion_s;

ARM_Motion_s calculate_forward(const float *cartesian);
ARM_Motion_s calculate_arm(const float *cartesian);

void Arm_motion_reset(void);

extern char ARM_init_script[];

#endif

#endif
