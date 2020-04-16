
#include "proj/tl_common.h"
#include "proj/mcu/clock.h"

#include "../config/user_config.h"
#include "../mcu/config.h"
#include "../mcu/clock_i.h"

#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258))

#include "../drivers/adc.h"
#include "analog.h"
#include "clock.h"
#include "gpio.h"
#include "../common/compatibility.h"

_attribute_data_retention_	unsigned int rnd_m_w = 0;
_attribute_data_retention_	unsigned int rnd_m_z = 0;

//unsigned int AAA_mz;  //debug
//unsigned int AAA_mw;



unsigned int tick_rand_adc_begin;
void random_generator_pre_init(void)
{
	adc_reset_adc_module();

	/******enable signal of 24M clock to sar adc********/
	adc_enable_clk_24m_to_sar_adc(1);

	/******set adc clk as 4MHz******/
	adc_set_sample_clk(5);



	//set R_max_mc,R_max_c,R_max_s
//	adc_set_state_length(80, 0, 0);  	//max_mc  80:f=300K T=3.3us
	adc_set_state_length(80, 96, 6);

	//set total length for sampling state machine and channel
	adc_set_chn_enable_and_max_state_cnt(ADC_RNS_CHN, 0);
	//set channel Vref
	adc_set_ref_voltage(ADC_MISC_CHN, ADC_VREF_0P6V);

	////set RNG src
	RNG_Set(SAR_ADC_RNG_MODE,CLOCLK_UPDATA);


	adc_power_on_sar_adc(1);		//After setting the ADC parameters, turn on the ADC power supply control bit

//	tick_rand_adc_begin = clock_time();
}





void random_generator_init(void)
{
//	while( !clock_time_exceed(tick_rand_adc_begin, 335) )  //T=3.3us, 100 clock

	rnd_m_w = RNG_Read()<<16 | RNG_Read();
	rnd_m_z = RNG_Read()<<16 | RNG_Read();

//	AAA_mz = rnd_m_z;  //debug
//	AAA_mw = rnd_m_w;

	adc_power_on_sar_adc(0);		//After setting the ADC parameters, turn on the ADC power supply control bit
}


//16M clock, code in flash 23us, code in sram 4us
_attribute_ram_code_ unsigned int rand(void)
{
	rnd_m_w = 18000 * (rnd_m_w & 0xffff) + (rnd_m_w >> 16);
	rnd_m_z = 36969 * (rnd_m_z & 0xffff) + (rnd_m_z >> 16);
	unsigned int result = (rnd_m_z << 16) + rnd_m_w;

	return (unsigned int)( result ^ clock_time() );
}




/*********************************************************************
 * @fn          generateRandomNum
 *
 * @brief       generate random number
 *
 * @param       len - len
 *
 * @param       data -  buffer
 *
 * @return      None
 */
void generateRandomNum(int len, unsigned char *data)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for (i=0; i<len; i++ ) {
		if( (i & 3) == 0 ){
			randNums = rand();
		}

		data[i] = randNums & 0xff;
		randNums >>=8;
	}
}
#endif

