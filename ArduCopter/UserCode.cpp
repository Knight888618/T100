#include "Copter.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
	if(!userhook.initialised)
	{
		userhook.local_port = AP::serialmanager().find_serial(AP_SerialManager::SerialProtocol_WIT, 0);//查找本地端口

		if(userhook.local_port)
		{
			userhook.initialised = true;
		}
	}
}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here
}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
	uint16_t num_byte=0;
	uint8_t check_sum=0;
	uint8_t data_byte=0;
	uint8_t buffer_index = 0;
	uint8_t frame_flag=0;
    uint8_t data_buffer_r[100];																	//接收缓冲

	if(userhook.initialised)
	{
		num_byte = userhook.local_port->available();												//读取缓冲区可用字节数

		while(num_byte--)																			//一次读取一个字节
		{
			data_byte = userhook.local_port->read();												//读取一个字节

			if(frame_flag==0)																		//初始同步标志
			{
				if((data_byte == 0x55) && (buffer_index==0))										//读到帧头1
				{
					data_buffer_r[buffer_index++] = data_byte;										//保存到缓冲区
					continue;																		//退出本次循环
				}

				if((data_byte == 0x53) && (buffer_index==1))										//读取下一个字节，如果读到帧头0xBB
				{
					data_buffer_r[buffer_index++] = data_byte;										//保存到缓冲区
					frame_flag = 1;
					continue;																		//退出本次循环
				}
			}
			if(frame_flag == 1)
			{
				data_buffer_r[buffer_index++] = data_byte;											//保存非帧头的数据字节

				if(buffer_index == 11)																//读取到一帧完整数据
				{
					for(buffer_index=0; buffer_index<10; buffer_index++)							//计算校验和
					{
						check_sum = check_sum + data_buffer_r[buffer_index];
					}

					if(check_sum == data_buffer_r[10])												//校验和匹配、收到的是从机要跟随的头机系统ID
					{
						short value = data_buffer_r[3] << 8 | data_buffer_r[2];
						userhook.angle_pitch = ((float)value/32768)*180.0f;
					}
				}
			}
		}
	}
}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
}

void Copter::userhook_auxSwitch2(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(const RC_Channel::AuxSwitchPos ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
