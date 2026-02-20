#include "max30102_read.h"
#include "max30102.h"
#include "algorithm.h"
#include "myiic.h"

#define MAX_BRIGHTNESS 255
int32_t hrAvg = 0;
int32_t spo2Avg = 0;
uint32_t aun_ir_buffer[150]; //infrared LED sensor data
uint32_t aun_red_buffer[150];  //red LED sensor data
int32_t n_ir_buffer_length; //data length
int32_t n_spo2;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;

int32_t hr_buf[16];
int32_t hrSum;
extern int32_t hrAvg;
int32_t spo2_buf[16];
int32_t spo2Sum;
extern int32_t spo2Avg;
int32_t spo2BuffFilled;
int32_t hrBuffFilled;
int32_t hrValidCnt = 0;
int32_t spo2ValidCnt = 0;
int32_t hrThrowOutSamp = 0;
int32_t spo2ThrowOutSamp = 0;
int32_t spo2Timeout = 0;
int32_t hrTimeout = 0;
uint32_t un_min, un_max,un_prev_data;
uint32_t  un_brightness;  //variables to calculate the on-board LED brightness that reflects the heartbeats

void Init_MAX30102(void)
{
    int32_t i;

    un_brightness = 0;
    un_min = 0x3FFFF;
    un_max = 0;
    
	  bsp_InitI2C();//IIC≥ı ºªØ
	  maxim_max30102_reset(); //resets the MAX30102
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
    maxim_max30102_init();  //initialize the MAX30102
	
    n_ir_buffer_length = 150; //buffer length of 150 stores 3 seconds of samples running at 50sps

    //read the first 150 samples, and determine the signal range
    for(i = 0; i < n_ir_buffer_length; i++)
    {
        //while(KEY0 == 1); //wait until the interrupt pin asserts
       //read from MAX30102 FIFO
			  maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));
        if(un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i]; //update signal min
        if(un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i]; //update signal max
    }
    un_prev_data = aun_red_buffer[i];
    //calculate heart rate and SpO2 after first 150 samples (first 3 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

void ReadHeartRateSpO2(void)
{
	  int32_t i;
	  float f_temp;
	  static u8 COUNT=8;
	
		i = 0;
		un_min = 0x3FFFF;
		un_max = 0;

		//dumping the first 50 sets of samples in the memory and shift the last 100 sets of samples to the top
		for(i = 50; i < 150; i++)
		{
				aun_red_buffer[i - 50] = aun_red_buffer[i];
				aun_ir_buffer[i - 50] = aun_ir_buffer[i];

				//update the signal min and max
				if(un_min > aun_red_buffer[i])
						un_min = aun_red_buffer[i];
				if(un_max < aun_red_buffer[i])
						un_max = aun_red_buffer[i];
		}

		//take 50 sets of samples before calculating the heart rate.
		for(i = 100; i < 150; i++)
		{
				un_prev_data = aun_red_buffer[i - 1];
				maxim_max30102_read_fifo((aun_ir_buffer+i), (aun_red_buffer+i));  //read from MAX30102 FIFO

				//calculate the brightness of the LED
				if(aun_red_buffer[i] > un_prev_data)
				{
						f_temp = aun_red_buffer[i] - un_prev_data;
						f_temp /= (un_max - un_min);
						f_temp *= MAX_BRIGHTNESS;
						f_temp = un_brightness - f_temp;
						if(f_temp < 0)
								un_brightness = 0;
						else
								un_brightness = (int)f_temp;
				}
				else
				{
						f_temp = un_prev_data - aun_red_buffer[i];
						f_temp /= (un_max - un_min);
						f_temp *= MAX_BRIGHTNESS;
						un_brightness += (int)f_temp;
						if(un_brightness > MAX_BRIGHTNESS)
								un_brightness = MAX_BRIGHTNESS;
				}
		}
		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		if(COUNT++ > 2)
		{
					COUNT = 0;
			
					if ((ch_hr_valid == 1) && (n_heart_rate < 150) && (n_heart_rate > 60))
					{
							hrTimeout = 0;

							// Throw out up to 1 out of every 5 valid samples if wacky
							if (hrValidCnt == 4)
							{
									hrThrowOutSamp = 1;
									hrValidCnt = 0;
									for (i = 12; i < 16; i++)
									{
											if (n_heart_rate < hr_buf[i] + 10)
											{
													hrThrowOutSamp = 0;
													hrValidCnt   = 4;
											}
									}
							}
							else
							{
									hrValidCnt = hrValidCnt + 1;
							}

							if (hrThrowOutSamp == 0)
							{

									// Shift New Sample into buffer
									for(i = 0; i < 15; i++)
									{
											hr_buf[i] = hr_buf[i + 1];
									}
									hr_buf[15] = n_heart_rate;

									// Update buffer fill value
									if (hrBuffFilled < 16)
									{
											hrBuffFilled = hrBuffFilled + 1;
									}

									// Take moving average
									hrSum = 0;
									if (hrBuffFilled < 2)
									{
											hrAvg = 0;
									}
									else if (hrBuffFilled < 4)
									{
											for(i = 14; i < 16; i++)
											{
													hrSum = hrSum + hr_buf[i];
											}
											hrAvg = hrSum >> 1;
									}
									else if (hrBuffFilled < 8)
									{
											for(i = 12; i < 16; i++)
											{
													hrSum = hrSum + hr_buf[i];
											}
											hrAvg = hrSum >> 2;
									}
									else if (hrBuffFilled < 16)
									{
											for(i = 8; i < 16; i++)
											{
													hrSum = hrSum + hr_buf[i];
											}
											hrAvg = hrSum >> 3;
									}
									else
									{
											for(i = 0; i < 16; i++)
											{
													hrSum = hrSum + hr_buf[i];
											}
											hrAvg = hrSum >> 4;
									}
							}
							hrThrowOutSamp = 0;
					}
					else
					{
							hrValidCnt = 0;
							if (hrTimeout == 2)
							{
									hrAvg = 0;
									hrBuffFilled = 0;
							}
							else
							{
									hrTimeout++;
							}
					}

					if ((ch_spo2_valid == 1) && (n_spo2 > 80))
					{
							spo2Timeout = 0;

							// Throw out up to 1 out of every 5 valid samples if wacky
							if (spo2ValidCnt == 4)
							{
									spo2ThrowOutSamp = 1;
									spo2ValidCnt = 0;
									for (i = 12; i < 16; i++)
									{
											if (n_spo2 > spo2_buf[i] - 10)
											{
													spo2ThrowOutSamp = 0;
													spo2ValidCnt   = 4;
											}
									}
							}
							else
							{
									spo2ValidCnt = spo2ValidCnt + 1;
							}

							if (spo2ThrowOutSamp == 0)
							{

									// Shift New Sample into buffer
									for(i = 0; i < 15; i++)
									{
											spo2_buf[i] = spo2_buf[i + 1];
									}
									spo2_buf[15] = n_spo2;

									// Update buffer fill value
									if (spo2BuffFilled < 16)
									{
											spo2BuffFilled = spo2BuffFilled + 1;
									}

									// Take moving average
									spo2Sum = 0;
									if (spo2BuffFilled < 2)
									{
											spo2Avg = 0;
									}
									else if (spo2BuffFilled < 4)
									{
											for(i = 14; i < 16; i++)
											{
													spo2Sum = spo2Sum + spo2_buf[i];
											}
											spo2Avg = spo2Sum >> 1;
									}
									else if (spo2BuffFilled < 8)
									{
											for(i = 12; i < 16; i++)
											{
													spo2Sum = spo2Sum + spo2_buf[i];
											}
											spo2Avg = spo2Sum >> 2;
									}
									else if (spo2BuffFilled < 16)
									{
											for(i = 8; i < 16; i++)
									 		{
													spo2Sum = spo2Sum + spo2_buf[i];
											}
											spo2Avg = spo2Sum >> 3;
									}
									else
									{
											for(i = 0; i < 16; i++)
											{
													spo2Sum = spo2Sum + spo2_buf[i];
											}
											spo2Avg = spo2Sum >> 4;
									}
							}
							spo2ThrowOutSamp = 0;
					}
					else
					{
							spo2ValidCnt = 0;
							if (spo2Timeout == 2)
							{
									spo2Avg = 0;
									spo2BuffFilled = 0;
							}
							else
							{
									spo2Timeout++;
							}
					}
		}
}

