#include "Copter.h"

#if MODE_LOITER_ENABLED

bool ModeParaglid::init(bool ignore_checks)														// loiter_init - initialise loiter controller
{
    if (!copter.failsafe.radio)																	//没失控保护
    {
        float target_roll, target_pitch;

        update_simple_mode();																	// apply SIMPLE mode transform to pilot inputs

        get_pilot_desired_lean_angles(target_roll, target_pitch, loiter_nav->get_angle_max_cd(), attitude_control->get_althold_lean_angle_max_cd());// convert pilot input to lean angles

        loiter_nav->set_pilot_desired_acceleration(target_roll, target_pitch);					// process pilot's roll and pitch input
    } else
    {
        loiter_nav->clear_pilot_desired_acceleration();											// clear out pilot desired acceleration in case radio failsafe event occurs and we do not switch to RTL for some reason
    }

    loiter_nav->init_target();																	//将当前位置初始化为目标位置

    if (!pos_control->is_active_z())															// initialise the vertical position controller
    {
        pos_control->init_z_controller();
    }

    pos_control->set_max_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);// set vertical speed and acceleration limits
    pos_control->set_correction_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);

	_ctrl_alt = copter.g2.user_parameters.get_ctrl_alt();
	_sen_angle = copter.g2.user_parameters.get_sen_angle_max();
	_ctrl_angle1 = copter.g2.user_parameters.get_ctrl_angle1();
	_ctrl_angle2 = copter.g2.user_parameters.get_ctrl_angle2();
	_angle_rate = copter.g2.user_parameters.get_ctrl_rate();

	gcs().send_text(MAV_SEVERITY_NOTICE, "Ctrl Angle1=%f,Ctrl Angle2=%f\r\n", _ctrl_angle1, _ctrl_angle2);

	_channel_ctrl_state = 0x00;																	//初始化状态

	if (!copter.failsafe.radio)																	//未失控保护
	{
		for(uint8_t i=6; i<8; i++)																//7、8通道pwm
		{
			RC_Channel *rc_channel = rc().channel(i);
			_pwm_value[i-6] = rc_channel->get_radio_in();
		}

		//_pwm_value[0] = copter.g2.user_parameters.get_ch7_pwm();
		//_pwm_value[1] = copter.g2.user_parameters.get_ch8_pwm();

		if((_pwm_value[0] <1500) && (_pwm_value[1] <1500))										//初始进入模式需要7、8通道处于低位才有效
		{
			_channel_ctrl_state = 0x01;
			gcs().send_text(MAV_SEVERITY_NOTICE, "Angle CTRL Active");
		}else
		{
			gcs().send_text(MAV_SEVERITY_NOTICE, "Angle CTRL Inactive");
			return false;
		}
	}else
	{
		gcs().send_text(MAV_SEVERITY_NOTICE, "Radio FailSafe\r\n");
		return false;
	}

	gcs().send_text(MAV_SEVERITY_NOTICE, "Enter Mode 50");

    return true;
}

void ModeParaglid::run()																		// loiter_run - runs the loiter controller, should be called at 100hz or more
{
    float target_roll, target_pitch;
    float target_yaw_rate = 0.0f;
    float target_climb_rate = 0.0f;

    pos_control->set_max_speed_accel_z(-get_pilot_speed_dn(), g.pilot_speed_up, g.pilot_accel_z);// set vertical speed and acceleration limits

    if (!copter.failsafe.radio)																	// process pilot inputs unless we are in radio failsafe
    {
        update_simple_mode();																	// apply SIMPLE mode transform to pilot inputs

        get_pilot_desired_lean_angles(target_roll, target_pitch, loiter_nav->get_angle_max_cd(), attitude_control->get_althold_lean_angle_max_cd());// convert pilot input to lean angles

        target_yaw_rate = get_pilot_desired_yaw_rate();											// get pilot's desired yaw rate

        target_climb_rate = get_pilot_desired_climb_rate(channel_throttle->get_control_in());	// get pilot desired climb rate

        float posD;
        if (copter.ahrs_view->get_relative_position_D_origin(posD))
        {
        	if(-posD > _ctrl_alt)																//当前高度大于设定高度
        	{
        		target_climb_rate = 0.0f;
        		if(copter.userhook.angle_pitch < _sen_angle)
        		{
        			target_climb_rate = g.pilot_speed_up;
        		}

        		target_roll = 0.0f;
        		target_yaw_rate = 0.0f;
        	}
        }

    	for(uint8_t i=6; i<8; i++)																//7-8通道pwm
    	{
    		RC_Channel *rc_channel = rc().channel(i);
    		_pwm_value[i-6] = rc_channel->get_radio_in();
    	}

		//_pwm_value[0] = copter.g2.user_parameters.get_ch7_pwm();
		//_pwm_value[1] = copter.g2.user_parameters.get_ch8_pwm();

		if((_channel_ctrl_state == 0x01) && ((_pwm_value[0]>1500) && (_pwm_value[1]<1500)))		//初始都在低值，有效状态，通道7起作用设置角度1，条件是打到高位，同时通道8处于低位
		{
	    	target_pitch_angle = _ctrl_angle1;
	    	_channel_ctrl_state = 0x02;															//确认角度模式后退出
		}																						//第二次执行到加减模式

    	if((_pwm_value[1]>1500)	&& ((_channel_ctrl_state == 0x01) || (_channel_ctrl_state == 0x02)))//通道8处于到位直接设置角度2
    	{
    		target_pitch_angle = _ctrl_angle2;
    		_channel_ctrl_state = 0x03;
    	}

		if((_channel_ctrl_state == 0x02) || (_channel_ctrl_state == 0x03))
		{
	        RC_Channel *rc_channel = rc().channel(1);											//俯仰通道
			_pwm_value[2] = rc_channel->get_radio_in();											//俯仰通道pwm值
			if(_pwm_value[2] < 1350)															//大于1550开始加角度
			{
				target_pitch_angle -= _angle_rate/400.f;										//角速度换算
			}

			if(_pwm_value[2] > 1650)															//往回拉杆
			{
				target_pitch_angle += _angle_rate/400.f;										//减角度
				if(target_pitch_angle>0)
				{
					target_pitch_angle=0;
				}
			}

			target_pitch = target_pitch_angle*100.f;
		}

        target_climb_rate = constrain_float(target_climb_rate, -get_pilot_speed_dn(), g.pilot_speed_up);//限制爬升率

        loiter_nav->set_pilot_desired_acceleration(target_roll, target_pitch);					// process pilot's roll and pitch input
    } else
    {
        loiter_nav->clear_pilot_desired_acceleration();											// clear out pilot desired acceleration in case radio failsafe event occurs and we do not switch to RTL for some reason
    }

    if (copter.ap.land_complete_maybe) 															// relax loiter target if we might be landed
    {
        loiter_nav->soften_for_landing();
    }

    AltHoldModeState loiter_state = get_alt_hold_state(target_climb_rate);						// Loiter State Machine Determination

    switch (loiter_state)																		// Loiter State Machine
    {
    case AltHoldModeState::MotorStopped:
        attitude_control->reset_rate_controller_I_terms();
        attitude_control->reset_yaw_target_and_rate();
        pos_control->relax_z_controller(0.0f);   												// forces throttle output to decay to zero
        loiter_nav->init_target();
        attitude_control->input_thrust_vector_rate_heading(loiter_nav->get_thrust_vector(), target_yaw_rate, false);
        break;

    case AltHoldModeState::Landed_Ground_Idle:
        attitude_control->reset_yaw_target_and_rate();
        FALLTHROUGH;

    case AltHoldModeState::Landed_Pre_Takeoff:
        attitude_control->reset_rate_controller_I_terms_smoothly();
        loiter_nav->init_target();
        attitude_control->input_thrust_vector_rate_heading(loiter_nav->get_thrust_vector(), target_yaw_rate, false);
        pos_control->relax_z_controller(0.0f);   												// forces throttle output to decay to zero
        break;

    case AltHoldModeState::Takeoff:																// initiate take-off

        if (!takeoff.running())
        {
            takeoff.start(constrain_float(g.pilot_takeoff_alt,0.0f,1000.0f));
        }

        target_climb_rate = get_avoidance_adjusted_climbrate(target_climb_rate);				// get avoidance adjusted climb rate

        takeoff.do_pilot_takeoff(target_climb_rate);											// set position controller targets adjusted for pilot input

        loiter_nav->update();																	// run loiter controller

        attitude_control->input_thrust_vector_rate_heading(loiter_nav->get_thrust_vector(), target_yaw_rate, false);// call attitude controller
        break;

    case AltHoldModeState::Flying:
        motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);		// set motors to full range

        loiter_nav->update();

        attitude_control->input_thrust_vector_rate_heading(loiter_nav->get_thrust_vector(), target_yaw_rate, false);// call attitude controller

        target_climb_rate = get_avoidance_adjusted_climbrate(target_climb_rate);				// get avoidance adjusted climb rate

#if AP_RANGEFINDER_ENABLED
        copter.surface_tracking.update_surface_offset();										// update the vertical offset based on the surface measurement
#endif

        pos_control->set_pos_target_z_from_climb_rate_cm(target_climb_rate);					// Send the commanded climb rate to the position controller

        break;
    }

    pos_control->update_z_controller();															// run the vertical position controller and set output throttle
}

uint32_t ModeParaglid::wp_distance() const
{
    return loiter_nav->get_distance_to_target();
}

int32_t ModeParaglid::wp_bearing() const
{
    return loiter_nav->get_bearing_to_target();
}

#endif
