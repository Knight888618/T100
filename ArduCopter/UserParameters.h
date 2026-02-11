#pragma once

#include <AP_Param/AP_Param.h>

class UserParameters
{
public:
    UserParameters();
    static const struct AP_Param::GroupInfo var_info[];

    AP_Int16 get_ctrl_alt() const { return _ctrl_alt; }
    AP_Float get_sen_angle_min() const { return _sen_angle_min; }
    AP_Float get_sen_angle_max() const { return _sen_angle_max; }
    AP_Float get_ctrl_angle1() const { return _ctrl_angle1; }
    AP_Float get_ctrl_angle2() const { return _ctrl_angle2; }
    AP_Float get_ctrl_rate() const { return _ctrl_angle_rate; }
    AP_Float get_climb_rate_max() const { return _climb_rate_max; }
    AP_Float get_climb_rate_min() const { return _climb_rate_min; }
    AP_Float get_pitch_max() const { return _pitch_max; }
    AP_Float get_ctrl_angle0() const { return _ctrl_angle0; }

    AP_Int16 get2_ctrl_alt() const { return T_ctrl_alt; }
    AP_Float get2_sen_angle_min() const { return T_sen_ang_min; }
    AP_Float get2_sen_angle_max() const { return T_sen_ang_max; }
    AP_Float get2_ctrl_angle1() const { return T_ctrl_ang1; }
    AP_Float get2_ctrl_angle2() const { return T_ctrl_ang2; }
    AP_Float get2_ctrl_rate() const { return T_ctrl_ang_rate; }
    AP_Float get2_climb_rate_max() const { return T_climb_rate_max; }
    AP_Float get2_climb_rate_min() const { return T_climb_rate_min; }
    AP_Float get2_ctrl_angle0() const { return T_ctrl_angle0; }
    //AP_Int16 get_ch7_pwm() const { return _ch7_pwm; }
    //AP_Int16 get_ch8_pwm() const { return _ch8_pwm; }

private:
    AP_Int16 _ctrl_alt;
    AP_Float _sen_angle_max;
    AP_Float _sen_angle_min;
    AP_Float _ctrl_angle1;
    AP_Float _ctrl_angle2;
    AP_Float _ctrl_angle_rate;
    AP_Float _climb_rate_max;
    AP_Float _climb_rate_min;
    AP_Float _pitch_max;
    AP_Float _ctrl_angle0;


    AP_Int16 T_ctrl_alt;
    AP_Float T_sen_ang_max;
    AP_Float T_sen_ang_min;
    AP_Float T_ctrl_ang1;
    AP_Float T_ctrl_ang2;
    AP_Float T_ctrl_ang_rate;
    AP_Float T_climb_rate_max;
    AP_Float T_climb_rate_min;
    AP_Float T_ctrl_angle0;
    //AP_Int16 _ch7_pwm;
    //AP_Int16 _ch8_pwm;
};
/* 
	AP_GROUPINFO("2_CTRL_ALT", 10, UserParameters, T_ctrl_alt, 20),
    AP_GROUPINFO("2_SEN_ANGLMAX", 11, UserParameters, T_sen_ang_max, -70),
	AP_GROUPINFO("2_SEN_ANGLMIN", 12, UserParameters, T_sen_ang_min, -90),
	AP_GROUPINFO("2_PITCH_ANGL1", 13, UserParameters, T_ctrl_ang1, -10),
	AP_GROUPINFO("2_PITCH_ANGL2", 14, UserParameters, T_ctrl_ang2, -20),
	AP_GROUPINFO("2_PITCH_RATE", 15, UserParameters, T_ctrl_ang_rate, 1),
	AP_GROUPINFO("2_CLIMB_MAX", 16, UserParameters, T_climb_rate_max, 500),
	AP_GROUPINFO("2_CLIMB_MIN", 17, UserParameters, T_climb_rate_min, 200),
	AP_GROUPINFO("2_PITCH_MAX", 18, UserParameters, T_pitch_max, 3000),
	AP_GROUPINFO("2_PITCH_ANGL0", 19, UserParameters, T_ctrl_angle0, -5),
    */