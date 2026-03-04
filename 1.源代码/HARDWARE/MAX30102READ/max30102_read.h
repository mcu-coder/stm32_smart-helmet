#ifndef MAX30102_READ_H
#define MAX30102_READ_H

#include <stdint.h>

// 使用 extern 声明全局变量
extern int32_t hrAvg;
extern int32_t spo2Avg;
extern uint32_t aun_ir_buffer[150];
extern uint32_t aun_red_buffer[150];  
extern int32_t n_ir_buffer_length;

// 函数声明
void Init_MAX30102(void);
void ReadHeartRateSpO2(void);

#endif /* MAX30102_READ_H */

