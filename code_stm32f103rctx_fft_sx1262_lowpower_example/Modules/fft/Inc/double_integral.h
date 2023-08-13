/**
 **************************************************************************
 *  @filename	频域一次/二次积分
 *  @author  	L-KAYA
 *  @date    	27-July-2018
 *  @brief   	该文件主要用于实现加速度在频域的一次或二次积分得到速度或位移，
 * 				有带通滤波功能，积分后漂移较小建议采样频率1kHz以上，效果较为理想
 *
 *  @example	integ_init(1024);
 *				float data_buf[1000];		//需要进行积分的数据数组
 *				float integ_buf[1024];		//用于储存积分结果的数组
 *				frq_domain_integral(data_buf, integ_buf);

 *  @attention	遇到malloc失败进入hardfault时，尝试把starup_stm32f40_41xxx.s
 * 				中的Stack_Size和Heap_Size调大，默认的堆栈比较小，数组太长时会
 * 				分配失败
 * 				该文件需要用到STM32F4的DSP库
 *
 * 	@other		该文件是参考王济《matlab在振动信号处理中的应用》的.m程序，适用
 * 				于振动环境（总位移为0）的位移计算，不适用于单次位移的测量
 **************************************************************************
 */

#ifndef _DOUBLE_INTEGRAL_H_
#define _DOUBLE_INTEGRAL_H_

#include <stdlib.h>
#include "math.h"
#include "arm_math.h" //stm32f4xx DSP库，带FPU，硬件加速FFT

// 函数接口
#define MALLOC(size) malloc(size)
#define MEMSET(addr, val, len) memset(addr, val, len)
#define POW(n, x) pow(n, x)

#define DATA_LEN 1024 // 数据长度，初始化时作为参数传入，2的幂次最佳

#define SMPLE_FRQ 60000 // 采样频率
#define GRAVITY 1		// 单位变化系数（最后积分结果乘上该系数，根据重力加速度得到）
#define INTEG_TIME 1	// 积分次数
#define FRQ_MIN 2		// 高通滤波截止频率
#define FRQ_MAX 30000	// 低通滤波截止频率（应小于采样频率）

typedef struct dou_integ
{
	uint8_t integ_time; // 积分次数，1为速度，2为距离
	uint16_t fft_len;	// FFT数组长度，为2的整数幂次
	float smple_frq;	// 采样频率，单位Hz
	float high_pass;	// 高通频率起始点
	float low_pass;		// 低通频率截止点
	float frq_min;		// 最小截止频率
	float frq_max;		// 最大截止频率
	float gravity;		// 单位变换系数
	float *w_vec;		// 频率向量
	float *fft_buf;		// 用于存储fft计算过程中的临时数组

	arm_cfft_instance_f32 S; // 用于fft计算的结构体

} dou_integ_t;

/**
 *  @name	频域一次/二次积分初始化函数
 *  @brief  初始化用于存储函数的结构体，下方可更改部分根据实际情况更改
 *  @pram	len	用于二次积分的数组长度，建议大于16小于4096，2的幂次
 *  @retval	0	成功
 *  		else失败
 */
int integ_init(uint16_t len, uint8_t integ_time);

/**
 *  @name	频域一次/二次积分去初始化函数
 *  @brief	不需要用到频域积分后可去初始化，释放内存空间
 *  @pram	无
 *  @retval	无
 */
void integ_deinit(void);

/**
 *  @name	频域一次/二次积分函数
 *  @brief	计算一次/二次积分
 *  @pram	inputbuf	输入数组
 *  		outputbuf	输出数组（长度大于等于输入数组且需为2^(4~12)）
 *  @retval	0			成功
 *  		else		失败
 */
int frq_domain_integral(float *inputbuf, float *outputbuf, float *Mag, float *freq);

#endif
