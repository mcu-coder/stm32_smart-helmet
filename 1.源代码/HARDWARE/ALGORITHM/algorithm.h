#ifndef ALGORITHM_H_
#define ALGORITHM_H_
#include "stm32f10x.h"
#include "stdbool.h"

#define true 1
#define false 0
#define FS 50    //sampling frequency
#define BUFFER_SIZE  (FS* 3) 
#define MA4_SIZE  4 // DONOT CHANGE
#define min(x,y) ((x) < (y) ? (x) : (y))

static  int32_t an_x[ BUFFER_SIZE]; //ir
static  int32_t an_y[ BUFFER_SIZE]; //red

// 确保这个函数声明存在
int8_t maxim_finger_detected(uint32_t *pun_ir_buffer, int32_t n_buffer_length);

void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, int8_t *pch_hr_valid);
void maxim_find_peaks(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height);
void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
void maxim_sort_ascend(int32_t  *pn_x, int32_t n_size);
void maxim_sort_indices_descend(int32_t  *pn_x, int32_t *pn_indx, int32_t n_size);

#endif /* ALGORITHM_H_ */

