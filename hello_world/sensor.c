#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

#include "app_util.h"
#include "dev/adc.h"
#include "dev/leds.h"
#include "dev/i2c.h"
#include "dev/mpu-6050.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120
#define RESOLUTION 7


#define ADC_SAMPLING_FREQ 64 //use power of 2 in Hz (tested 1, 2, 4...32)
#define ADC_SAMPLES_PER_FRAME (ADC_SAMPLING_FREQ/FRAMES_PER_SEC)

#define MPU_SAMPLING_FREQ 256 //tested 1, 2, and 4
#define MPU_SAMPLES_PER_FRAME (MPU_SAMPLING_FREQ/FRAMES_PER_SEC_INT)

#define I2C_SENSOR
//#define ADC_SENSOR

#ifdef I2C_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ MPU_SAMPLING_FREQ
#define SAMPLES_PER_FRAME MPU_SAMPLES_PER_FRAME
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));
#endif /*I2C_SENSOR*/

#ifdef ADC_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ ADC_SAMPLES_PER_FRAME
#define SAMPLES_PER_FRAME ADC_SAMPLES_PER_FRAME
#endif /*ADC_SENSOR*/

#ifndef DATA_SIZE
#define DATA_SIZE 1 //size of uint8_t (1 bit)
#endif /*DATA_SIZE*/





static const int8_t SIN_TAB[] =
{
		0,6,13,20,26,33,39,45,52,58,63,69,75,80,
		85,90,95,99,103,107,110,114,116,119,121,
		123,125,126,127,127,127,127,127,126,125,
		123,121,119,116,114,110,107,103,99,95,90,
		85,80,75,69,63,58,52,45,39,33,26,20,13,6,
		0,-6,-13,-20,-26,-33,-39,-45,-52,-58,-63,
		-69,-75,-80,-85,-90,-95,-99,-103,-107,-110,
		-114,-116,-119,-121,-123,-125,-126,-127,-127,
		-127,-127,-127,-126,-125,-123,-121,-119,-116,
		-114,-110,-107,-103,-99,-95,-90,-85,-80,-75,
		-69,-63,-58,-52,-45,-39,-33,-26,-20,-13,-6
};

static int8_t sinI(uint16_t angleMilli)
{
	uint16_t pos;
	pos = (uint16_t) ((SIN_TAB_LEN * (uint32_t) angleMilli)/1000);
	return SIN_TAB[pos%SIN_TAB_LEN];
}

static int8_t sin(uint16_t angleMilli)
{
	return SIN_TAB[angleMilli%SIN_TAB_LEN];
}

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Hello world Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
//APP Callback function
static void app_recv(void)
{
	//printf("Received from RDC\n");
	PROCESS_CONTEXT_BEGIN(&null_app_process);
	
	//int16_t *data = (int16_t*)packetbuf_dataptr();

#ifdef SF_MOTE_TYPE_AP
	mpu_data_union data[SAMPLES_PER_FRAME];



	//int8_t *raw_data = (int8_t *)packetbuf_dataptr();

	uint8_t i;
	rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
	uint8_t rx_sn_id = sent_sn_addr->u8[0];

	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	//DATA_SIZE;
	uint8_t payload_len = packetbuf_datalen();

	if(payload_len != MPU_DATA_SIZE*SAMPLES_PER_FRAME)
	{
		printf("Ignore packet %u,%u %u\n",rx_sn_id,pkt_seq,payload_len);
		return;
	}
	else
	{
		memcpy((uint8_t*)data,packetbuf_dataptr(),MPU_DATA_SIZE*SAMPLES_PER_FRAME);
	}


	


//	printf("%u,%u,",rx_sn_id,pkt_seq);
/*
	for(i = 0;i < payload_len;i++)
	{
		printf("%02x,",raw_data[i]);
	}
	printf("\n");
*/	
	for(i=0;i<SAMPLES_PER_FRAME;i++)
	{
		// MPU data
		mpu_data_union samples = data[i];
		//print result
		
		//printf("%d,%d,%d,%d,%d,%d,%d\n",sampled_data.data.accel_x,sampled_data.data.accel_y,sampled_data.data.accel_z,sampled_data.data.temperature,sampled_data.data.gyro_x,sampled_data.data.gyro_y,sampled_data.data.gyro_z);

		MPU_PRINT_BYTE(rx_sn_id);
		MPU_PRINT_BYTE(0);
		MPU_PRINT_BYTE(pkt_seq);
		MPU_PRINT_BYTE(0);
		MPU_PRINT_BYTE(samples.reg.x_accel_h);
		MPU_PRINT_BYTE(samples.reg.x_accel_l);
		MPU_PRINT_BYTE(samples.reg.y_accel_h);
		MPU_PRINT_BYTE(samples.reg.y_accel_l);
		MPU_PRINT_BYTE(samples.reg.z_accel_h);
		MPU_PRINT_BYTE(samples.reg.z_accel_l);
		MPU_PRINT_BYTE(samples.reg.x_gyro_h);
		MPU_PRINT_BYTE(samples.reg.x_gyro_l);
		MPU_PRINT_BYTE(samples.reg.y_gyro_h);
		MPU_PRINT_BYTE(samples.reg.y_gyro_l);
		MPU_PRINT_BYTE(samples.reg.z_gyro_h);
		MPU_PRINT_BYTE(samples.reg.z_gyro_l);
		MPU_PRINT_BYTE(samples.reg.t_h);
		MPU_PRINT_BYTE(samples.reg.t_l);
		MPU_PRINT_BYTE('\n');

	}

#endif
/*
	app_output_16t(data,rx_sn_id,pkt_seq,payload_len);
*/
	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};


/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();


	printf("Hello world Started.\n");

#ifdef SF_FEATURE_SHELL_OPT
	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();
#endif

	app_conn_open(&nullApp_callback);


#ifdef ADC_SENSOR
	static uint16_t samples[ADC_SAMPLES_PER_FRAME]={0};
	uint8_t i;
//	static uint8_t samples_sorted_bytes[2*ADC_SAMPLES_PER_FRAME];
	static uint8_t sample_num = 0; //increments from 0 to samples_per_frame-1


	if (node_id != 0){
		adc_on();
		//adc_configure(0); //to sample reference voltage (Vref/2), ~2048.
		etimer_set( &rxtimer, (unsigned long)(CLOCK_SECOND/(ADC_SAMPLING_FREQ)));
	}
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	if(node_id != 0)
	{

	  while(1)
	  {

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
	    etimer_reset(&rxtimer);

	    samples[sample_num]=adc_sample();
	    sample_num++;
	    if(sample_num == ADC_SAMPLES_PER_FRAME){
	    	sample_num=0;
	    	/*
	    	 * Byte order needs to be reversed because of low-endian system.
	    	 * Can be done at AP level too, if needed.
	    	 */
	    	// for(i=0;i<ADC_SAMPLES_PER_FRAME;i++){
	    	// 	samples_sorted_bytes[2*i]=(samples[i]>>8);
	    	// 	samples_sorted_bytes[2*i+1]= (samples[i]& 0xff);
	    	// }


	    	//app_conn_send(samples_sorted_bytes,sizeof(uint8_t)*ADC_SAMPLES_PER_FRAME*2);
	    	 tdma_rdc_buf_ptr = 0;
	    	 tdma_rdc_buf_send_ptr = 0;
	    	 tdma_rdc_buf_full_flg = 0;
	    	 app_conn_send(samples,sizeof(uint16_t)*ADC_SAMPLES_PER_FRAME/sizeof(uint8_t));
	    }

	  }
	}
#endif


#ifdef I2C_SENSOR
	//static rtimer_clock_t rt, del;
	int i;
	static uint8_t MPU_status = 0;
	static uint8_t sample_count = 0;
/*
	static uint8_t samples_sorted_bytes[14*MPU_SAMPLES_PER_FRAME],comp_samples_sorted_bytes[14*MPU_SAMPLES_PER_FRAME];
	static uint8_t sample_num=0, uncomp_data_len=14*MPU_SAMPLES_PER_FRAME,comp_data_len;
	static uint8_t *st;
*/


	static mpu_data sampleArray[MPU_SAMPLES_PER_FRAME];

	if (node_id != 0){

		MPU_status = 0;
		for(i = 0; i < 100 & (~MPU_status);i++)
		{
			MPU_status = mpu_enable();
		}

		if (MPU_status == 0)
			printf("MPU could not be enabled.\n");


		MPU_status = 0;
		for(i = 0; i < 100 & (~MPU_status);i++)
		{
			MPU_status = mpu_wakeup();
		}

		if (MPU_status == 0)
			printf("MPU could not be awakened.\n");

		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/MPU_SAMPLING_FREQ));
		}
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	if(node_id != 0)
	{

		while(1){
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
			etimer_reset(&rxtimer);

			mpu_data_union samples;
			int m=mpu_sample_all(&samples);
			app_conn_send((uint8_t *)(&samples),MPU_DATA_SIZE);
/*
			sampleArray[sample_count] = samples.data;

			sample_count = sample_count + 1;

			if(sample_count == MPU_SAMPLES_PER_FRAME)
			{
				sample_count = 0;
				tdma_rdc_buf_clear();

				app_conn_send(sampleArray,MPU_DATA_SIZE*MPU_SAMPLES_PER_FRAME);

			}
*/
/*
			st = &samples;
			for(i=0;i<7;i++){
				samples_sorted_bytes[2*i+14*sample_num]=*(st+2*i+1);
				samples_sorted_bytes[2*i+1+14*sample_num]= *(st+2*i);
			}
			sample_num++;

			if(sample_num==MPU_SAMPLES_PER_FRAME){
				sample_num=0;

				app_conn_send(samples_sorted_bytes,sizeof(uint8_t)*14*MPU_SAMPLES_PER_FRAME);
			}

			PRINTF("%d,%d,%d,%d,%d,%d,%d\n",samples.data.accel_x,samples.data.accel_y,samples.data.accel_z,samples.data.gyro_x,samples.data.gyro_y,
				samples.data.gyro_z,samples.data.temperature);
*/
//			app_conn_send(&samples,sizeof(mpu_data)/sizeof(uint8_t));
		}
	}

#endif

	PROCESS_END();

}
