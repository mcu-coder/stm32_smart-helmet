#include "main.h"



#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

 SensorModules sensorData;								//声明传感器数据结构体变量
 SensorThresholdValue Sensorthreshold;		//声明传感器阈值结构体变量
 DriveModules driveData;									//声明驱动器状态结构体变量

uint8_t mode = 1;	//系统模式  1自动  2手动  3设置
u8 dakai;//串口3使用的传递变量
u8 Flag_dakai;//串口3接收标志位
uint8_t is_secondary_menu = 0;  // 0一级菜单，1二级菜单
uint8_t secondary_pos = 1;      // 二级菜单光标位置（1-3对应时/分/秒）
uint8_t secondary_type = 0;   // 二级菜单类型：0=RTC时间，1=定时开启，2=定时关闭

uint8_t send_data[] = "";//语音播放曲目1
uint8_t send_data1[] = "";//语音播放曲目2

	extern unsigned char p[16];
	short temperature = 0; 	

//系统静态变量
//static uint8_t count_a = 1;  //自动模式按键数
static uint8_t count_m = 1;  //手动模式按键数
static uint8_t count_s = 1;	 //设置模式按键数
//static uint8_t last_mode = 0;      // 记录上一次的模式
//static uint8_t last_count_s = 0;   // 记录设置模式内上一次的页面
/**
  * @brief  显示菜单内容
  * @param  无
  * @retval 无
  */
enum 
{
	AUTO_MODE = 1,
	MANUAL_MODE,
	SETTINGS_MODE
	
}MODE_PAGES;

  
/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_autoPage1(void)		//自动模式菜单第一页
{
	
	//显示”心率
	OLED_ShowChinese(0,16,61,16,1); 
	OLED_ShowChinese(16,16,62,16,1);
	OLED_ShowChar(32,16,':',16,1);
	
	//显示“温度：  C”
	OLED_ShowChinese(0,0,0,16,1);	//温
	OLED_ShowChinese(16,0,2,16,1);	//度
	OLED_ShowChar(32,0,':',16,1);
	 
	
	OLED_Refresh();
	
}
void OLED_autoPage2(void)   //自动模式菜单第二页
{
	

}



void SensorDataDisplay1(void)		//传感器数据显示第一页
{
    SensorScan();	//获取传感器数据
    char all_data[128];  // 足够大的缓冲区容纳所有数据
   const char* vib_status = (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) ? "是" : "否";
    
    // 一次性格式化所有数据到同一个字符串
    sprintf(all_data, " 温度: %.1f\r\n 心率: %d \r\n 光照: %d\r\n  距离：%d CM\r\n 是否碰撞: %s\r\n",
        sensorData.temp,  // p 是字符串，用 %s 输出
        sensorData.hrAvg,
        sensorData.lux,
        sensorData.distance,
        vib_status);
    
    USART2_SendString(all_data);  // 一次发送所有数据
    //delay_ms(100); 

    //显示温度数据
		OLED_ShowString(40,0,(u8*)p ,16,1);
 
   
}

void SensorDataDisplay2(void)		//传感器数据显示第二页
{


}

/**
  * @brief  显示手动模式设置界面1
  * @param  无
  * @retval 无
  */
void OLED_manualPage1(void)
{
	//显示“灯光”
	OLED_ShowChinese(16,0,28,16,1);	
	OLED_ShowChinese(32,0,29,16,1);	
	OLED_ShowChar(64,0,':',16,1);
 
	
}

/**
  * @brief  显示手动模式设置参数界面1
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay1(void)
{
	if(driveData.LED_Flag ==1)
	{
		OLED_ShowChinese(96,0,40,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,0,42,16,1); 	//关
	}
	 
}

/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{

	
	//显示“温度阈值”
	OLED_ShowChinese(16,0,0,16,1);	
	OLED_ShowChinese(32,0,2,16,1);	
	OLED_ShowChinese(48,0,26,16,1);	
	OLED_ShowChinese(64,0,27,16,1);	
	OLED_ShowChar(80,0,':',16,1);
 
	
}

void OLED_settingsPage2(void)//显示系统阈值设置界面2
{


}
void OLED_settingsPage3(void)//显示系统阈值设置界面3
{

}

void SettingsThresholdDisplay1(void)//实际阈值1
{
	//显示温度阈值数值
	OLED_ShowNum(90,0, Sensorthreshold.tempValue, 2,16,1);
	 
}

void SettingsThresholdDisplay2(void)//实际阈值2
{

}

void SettingsThresholdDisplay3(void)//实际阈值3
{

}

/**
  * @brief  记录自动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetAuto(void)  
{

 return 1;  // 自动模式只有一页，始终返回1
}

/**
  * @brief  记录手动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetManual(void)  
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_m++;
//		if (count_m == 1)
//		{
//			OLED_Clear();
//			
//		}
		if (count_m > 2)  		//一共可以控制的外设数量
		{
			OLED_Clear();
			count_m = 1;
		}
	}
	return count_m;
}

/**
  * @brief  记录阈值界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{

    if(KeyNum == KEY_2 && is_secondary_menu == 0)
    {
        KeyNum = 0;
        count_s++;
        if (count_s > 4)
        {
            count_s = 1;
        }


    }
    return count_s;
}

/**
  * @brief  显示手动模式界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_manualOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			default: break;
	}
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_settingsOption(uint8_t num)
{
	static uint8_t prev_num = 1; 

    // 清除上一次光标（仅操作光标位置，不影响数据）
    switch(prev_num)
    {
        case 1: OLED_ShowChar(0, 0, ' ', 16, 1); break; // 系统时间行
        case 2: OLED_ShowChar(0, 16, ' ', 16, 1); break; // 温度阈值行
        case 3: OLED_ShowChar(0, 32, ' ', 16, 1); break; // 湿度阈值行
        case 4: OLED_ShowChar(0, 48, ' ', 16, 1); break; // 光照阈值行
//        case 5: OLED_ShowChar(0, 16, ' ', 16, 1); break; // 定时开启行
//        case 6: OLED_ShowChar(0, 48, ' ', 16, 1); break; // 定时关闭行
        default: break;
    }
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		 

		default: break;
	}
	 prev_num = num;
    OLED_Refresh(); // 仅刷新光标，数据区域无变化
}

/**
  * @brief  自动模式控制函数
  * @param  无
  * @retval 无
  */
void AutoControl(void)//自动控制
{
	if( (sensorData.temp > Sensorthreshold.tempValue)    // 温度超标
 || (sensorData.hrAvg > Sensorthreshold.hrAvgValue)    // 湿度超标
  )                            // 一氧化碳超标（假设1=超标）
  {
   driveData.BEEP_Flag =1; // 任一超标，需要开蜂鸣器
  }
  else
	driveData.BEEP_Flag =0;
		
    

		
		FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, Sensorthreshold.hrAvgValue,
                    Sensorthreshold.luxValue, Sensorthreshold.distanceValue);
}

/**
  * @brief  手动模式控制函数
  * @param  无
  * @retval 无
  */
void ManualControl(uint8_t num)
{
	switch(num)
	{
		case 1:  
            if(KeyNum == KEY_3)
            {
                driveData.LED_Flag = 1;  
                KeyNum = 0;  
                printf("[按键] KEY3按下，LED_Flag置1\n");  
            }
            if(KeyNum == KEY_4)
            {
                driveData.LED_Flag = 0; 
                KeyNum = 0;  
                printf("[按键] KEY4按下，LED_Flag置0\n"); 
            }
            break;
 
		default: break;
	}
}

/**
  * @brief  控制函数
  * @param  无
  * @retval 无
  */
void Control_Manager(void)
{
    if(driveData.LED_Flag )
    {	
        LED_On(); 
    }
    
}

/**
  * @brief  阈值设置函数
  * @param  无
  * @retval 无
  */
void ThresholdSettings(uint8_t num)
{
	switch (num)
	{
		//温度
		case 1:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.tempValue += 1;
				if (Sensorthreshold.tempValue > 40)
				{
					Sensorthreshold.tempValue = 20;
				}
			}
		 
			break;
			//心率
			case 2:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.hrAvgValue += 1;
				if (Sensorthreshold.hrAvgValue > 120)
				{
					Sensorthreshold.hrAvgValue = 60;
				}
			}
		 
			break;
			//光照
			case 3:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.luxValue += 10;
				if (Sensorthreshold.luxValue > 500)
				{
					Sensorthreshold.luxValue = 80;
				}
			}
		  
			break;
		
        default: break;
	}
}
//flash读取
void FLASH_ReadThreshold()
{
	  Sensorthreshold.tempValue = FLASH_R(FLASH_START_ADDR);          // +0
    Sensorthreshold.hrAvgValue = FLASH_R(FLASH_START_ADDR + 2);     // +2
    Sensorthreshold.luxValue = FLASH_R(FLASH_START_ADDR + 4);       // +4
    Sensorthreshold.distanceValue = FLASH_R(FLASH_START_ADDR + 6);  // +6
	
}

int main(void)
{ 
    SystemInit();//配置系统时钟为72M	
    delay_init(72);  // 系统时钟72MHz
    ADCX_Init();
    LED_Init();
    BEEP_Init();
    
    //flash读取
    delay_ms(100);
    FLASH_ReadThreshold();
    vib_init();
    
    // 添加的状态管理变量
    static uint8_t last_mode = 0;  // 记录上一次模式
    static uint32_t last_sensor_time = 0; // 传感器扫描时间控制
    static uint32_t last_display_time = 0; // 显示刷新时间控制
    //超过以下值，恢复默认阈值
    if (Sensorthreshold.tempValue > 40 || Sensorthreshold.hrAvgValue >120 || 
        Sensorthreshold.distanceValue > 100 || Sensorthreshold.luxValue > 500)
    {
        FLASH_W(FLASH_START_ADDR, 30, 80, 100, 10);
        FLASH_ReadThreshold();
    }
    
    TIM2_Init(72-1,1000-1);  // 2ms定时中断
    printf("Start \n");
    USART3_SendString("AF:30");   //音量调到最大
    delay_ms(300);
    USART3_SendString("A7:00003");  //欢迎使用
    
    while (1)
    {	
        
        // ==================== 获取当前系统时间 ====================
        uint32_t current_time = delay_get_tick(); // 使用系统滴答计数
        
        // ==================== 优化传感器扫描频率 ====================
        if(current_time - last_sensor_time > 100) // 每200ms扫描一次传感器 (100 * 2ms = 200ms)
        {
            SensorScan(); 	//获取传感器数据
            last_sensor_time = current_time;
        }
        
        // ==================== 立即处理按键 ====================
        uint8_t current_key_num = KeyNum; // 保存当前按键值
        
        // 模式切换按键立即处理
        if(current_key_num != 0)
        {
           
        }
        
        // ==================== 模式切换优化 ====================
        if(last_mode != mode)
        {
            OLED_Clear();
            last_mode = mode;
            
            // 立即绘制新模式的固定内容
            switch(mode)
            {
                case AUTO_MODE:
                    OLED_autoPage1();
                    break;
                case MANUAL_MODE:
                    OLED_manualPage1();
                    break;
                case SETTINGS_MODE:
                    OLED_settingsPage1();
                    break;
            }
            OLED_Refresh(); // 立即刷新显示
        }
        
        // ==================== 模式处理 ====================
        switch(mode)
        {
            case AUTO_MODE:
                // 直接调用显示，不使用SetAuto()
                SensorDataDisplay1();	//显示传感器1数据
                AutoControl();
                Control_Manager();
                break;
                
            
            case SETTINGS_MODE:
            {
                // 优化设置模式响应速度
                static uint8_t is_threshold_page_inited = 0;
                uint8_t curr_count_s = SetSelection();
                
                // 立即处理设置模式内的按键
                if(current_key_num != 0)
                {
                    if (is_secondary_menu == 1)
                    {
                        // 二级菜单按键立即处理
                        if (current_key_num == KEY_2 || current_key_num == KEY_3 || current_key_num == KEY_4)
                        {
                            // 这里根据你的二级菜单逻辑处理
                            // 处理完后立即刷新
                            OLED_Refresh();
                            KeyNum = 0;
                        }
                        else if (current_key_num == KEY_1)
                        {
                            is_secondary_menu = 0;
                            secondary_pos = 1;
                            OLED_Clear();
                            OLED_settingsPage1();
                            SettingsThresholdDisplay1();
                            OLED_settingsOption(curr_count_s);
                            OLED_Refresh();
                            KeyNum = 0;
                        }
                    }
                    else
                    {
                        // 一级菜单按键立即处理
                        if (current_key_num == KEY_3 || current_key_num == KEY_4)
                        {
                            ThresholdSettings(curr_count_s);
                            SettingsThresholdDisplay1();
                            OLED_Refresh();
                            KeyNum = 0;
                        }
                        else if (current_key_num == KEY_1)
                        {
                            mode = AUTO_MODE;
                            is_threshold_page_inited = 0;
                            FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, Sensorthreshold.hrAvgValue,
                                    Sensorthreshold.luxValue, Sensorthreshold.distanceValue);
                            KeyNum = 0;
                        }
                    }
                }
                
                break;
            }
        }
        
        // ==================== 限制显示刷新频率 ====================
        if(current_time - last_display_time > 25) // 每50ms刷新一次显示 (25 * 2ms = 50ms)
        {
            // 所有模式都需要刷新显示
            OLED_Refresh();
            last_display_time = current_time;
        }
    }
}
