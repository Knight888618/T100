#pragma once

#include <AP_Param/AP_Param.h>

class UserParameters
{
public:
    UserParameters();
    static const struct AP_Param::GroupInfo var_info[];

    AP_Int8 get_ratclb_p() const { return _ratclb_p; }
    AP_Int16 get_ctrl_alt() const { return _ctrl_alt; }
    AP_Float get_sen_angle() const { return _sen_angle; }
    AP_Float get_ctrl_angle1() const { return _ctrl_angle1; }
    AP_Float get_ctrl_angle2() const { return _ctrl_angle2; }
    AP_Float get_ctrl_rate() const { return _angle_rate; }

    AP_Int16 get_ch7_pwm() const { return _ch7_pwm; }
    AP_Int16 get_ch8_pwm() const { return _ch8_pwm; }

private:
    AP_Int8 _ratclb_p;
    AP_Int16 _ctrl_alt;
    AP_Float _sen_angle;
    AP_Float _ctrl_angle1;
    AP_Float _ctrl_angle2;
    AP_Float _angle_rate;
    AP_Int16 _ch7_pwm;
    AP_Int16 _ch8_pwm;
};
