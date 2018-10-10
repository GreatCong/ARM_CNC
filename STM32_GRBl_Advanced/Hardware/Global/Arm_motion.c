#include "Arm_motion.h"
#include <string.h>

#include "grbl_advance.h"

#define BIG_ARM_LENGTH 148				//	@大臂长度	148mm
#define SMALL_ARM_LENGTH 160			//	@小臂长度	160mm
#define HEAD_OFFSET   50				//	@头偏移		50mm
#define CENCER_OFFSET 32				//	@中心偏移	32mm

#define ARM_OFFSET_X 100 //mm   		//	@X偏移	100mm
#define ARM_OFFSET_Y -56 //mm			//	@Y偏移	-56mm
#define ARM_OFFSET_Z -56 //mm			//	@Z偏移	-56mm
#define ARM_RAD2DEG 57.2957795			//	@转角	57.2957795度

extern int32_t sys_position[N_AXIS];

ARM_Motion_s arm_position_init;//笛卡尔坐标系为(0,0,0)的角度


#ifdef ARM
float return_angle(float a, float b, float c) {
  // cosine rule for angle between c and a
  return acos((a * a + c * c - b * b) / (2 * a * c));
}

//  @ fuction:  
//  @ description:  
//  @ input:
//  @ output:
//  @ note: 关节角度到空间位置
ARM_Motion_s calculate_forward(const float *cartesian_theta)//感觉有些问题
{
	float x1,y1;
	float x2,y2;
	
	ARM_Motion_s arm_motion_temp;
	
	y1=sin(cartesian_theta[X_AXIS]/ARM_RAD2DEG)*BIG_ARM_LENGTH-sin(cartesian_theta[Y_AXIS]/ARM_RAD2DEG)*SMALL_ARM_LENGTH;
	
	x1=cos(cartesian_theta[X_AXIS]/ARM_RAD2DEG)*BIG_ARM_LENGTH+cos(cartesian_theta[Y_AXIS]/ARM_RAD2DEG)*SMALL_ARM_LENGTH;
	x1=x1+HEAD_OFFSET+CENCER_OFFSET;
	
	x2 = x1 *cos(cartesian_theta[Z_AXIS]/ARM_RAD2DEG);
	y2 = x1 *sin(cartesian_theta[Z_AXIS]/ARM_RAD2DEG);
	
	arm_motion_temp.arm[X_AXIS]=x2;
	arm_motion_temp.arm[Y_AXIS]=y2;
	arm_motion_temp.arm[Z_AXIS]=y1;
	
  return arm_motion_temp;
}
  

//  @ fuction:  
//  @ description: 计算角度 
//  @ input:
//  @ output:
//  @ note: 空间位置到关节角度
ARM_Motion_s calculate_arm(const float *cartesian)
{
	float TempXY,TempXYZ;
	float TempX,TempY,TempZ;
	
  ARM_Motion_s arm_motion_temp;
	
	TempXY=sqrt(cartesian[X_AXIS]*cartesian[X_AXIS]
				+cartesian[Y_AXIS]*cartesian[Y_AXIS]);
	TempXYZ=sqrt((TempXY-CENCER_OFFSET-HEAD_OFFSET)*(TempXY-CENCER_OFFSET-HEAD_OFFSET)		
				+cartesian[Z_AXIS]*cartesian[Z_AXIS]);
	TempX = asin(cartesian[Z_AXIS]/(TempXYZ))+return_angle(BIG_ARM_LENGTH,SMALL_ARM_LENGTH,TempXYZ);
	TempY = return_angle(BIG_ARM_LENGTH,TempXYZ,SMALL_ARM_LENGTH);
	TempZ = atan(cartesian[Y_AXIS]/cartesian[X_AXIS]);
	
	
	arm_motion_temp.arm[X_AXIS] = TempX*ARM_RAD2DEG;//弧度转化为角度
	arm_motion_temp.arm[Y_AXIS] = 180-arm_motion_temp.arm[X_AXIS]-TempY*ARM_RAD2DEG;
	arm_motion_temp.arm[Z_AXIS] = TempZ*ARM_RAD2DEG;
	
	return arm_motion_temp;
	
  /*
  SERIAL_ECHOPGM("cartesian x="); SERIAL_ECHO(cartesian[X_AXIS]);
  SERIAL_ECHOPGM(" y="); SERIAL_ECHO(cartesian[Y_AXIS]);
  SERIAL_ECHOPGM(" z="); SERIAL_ECHOLN(cartesian[Z_AXIS]);

  SERIAL_ECHOPGM("delta x="); SERIAL_ECHO(delta[X_AXIS]);
  SERIAL_ECHOPGM(" y="); SERIAL_ECHO(delta[Y_AXIS]);
  SERIAL_ECHOPGM(" z="); SERIAL_ECHOLN(delta[Z_AXIS]);
  */
}

//  @ fuction:  
//  @ description: 角度复位 
//  @ input:
//  @ output:
//  @ note: 
void Arm_motion_reset(void){
//	float init_cartesian[N_AXIS];
//	memset(init_cartesian, 0 , sizeof(init_cartesian));
//init_cartesian[X_AXIS] = 90;
	
//  arm_position_init = calculate_arm(init_cartesian);
//	  arm_position_init = calculate_forward(init_cartesian);
	arm_position_init.arm[X_AXIS] = 90;
	arm_position_init.arm[Y_AXIS] = 30;
	arm_position_init.arm[Z_AXIS] = 0;
	
	for(int idx = 0; idx < 3; idx++) { //只用了3轴
		 sys_position[idx] = lround(arm_position_init.arm[idx]*settings.steps_per_mm[idx]);
  }
}

#endif
