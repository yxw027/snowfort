/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: hello-world.c,v 1.1 2006/10/02 21:46:46 adamdunkels Exp $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "i2c.h"
#include "spi.h"
#include <stdio.h> /* For printf() */
#include "sys/etimer.h"
#define debug
#define DUR (2500)
#define ADXL_ADDRESS 0xA6
#define ITG_ADDRESS 0xD0

  //unsigned char rv, xx0, xx1, yy0, yy1, zz0, zz1;
  int16_t accx, accy, accz;
  int16_t gyrx, gyry, gyrz;
  //float xx, yy, zz;
  
void init_adxl();
void init_itg();
void set_measure();
void measure_adxl();
void measure_itg();
void strange_measure();
void start();
void write(unsigned int _c);
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
	static struct etimer sensetimer;	
	etimer_set(&sensetimer, CLOCK_SECOND);
	//WDTCTL = WDTPW+WDTHOLD;
	// configuration -- bundle of single byte writes
	#ifdef debug
	printf("Configuring ADXL registers:\n");
	#endif
	init_adxl();
	init_itg();
	// Check the configuration registers:
	
	unsigned char configvector[29];
	unsigned char startregister = 0x1D;
	read_multibyte(ADXL_ADDRESS, startregister, 29, configvector);
	int ii;
	for(ii = 0; ii<29; ii++){
		printf("%d register: %d \n", ii+startregister, configvector[ii]);
	}

	startregister = 0x15;
	read_multibyte(ITG_ADDRESS, startregister, 3, configvector);
	for(ii = 0; ii<3; ii++){
		printf("%d register: %d \n", ii+startregister, configvector[ii]);
	}

	startregister = 0x3E;
	unsigned char rv;
	rv = read_(ITG_ADDRESS, startregister, 0);
	printf("%d register: %d \n", startregister, rv);

	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sensetimer));
		etimer_reset(&sensetimer);
		// testing i2c channel by reading whoami register, standard output should be 229
		#ifdef debug
		printf("Accessing WHOAMI register...\n");
		#endif
		rv = read_(ADXL_ADDRESS, 0x00, 0);
		
		if(rv == 229)
			printf("i2c working correctly, this sensor is 229\n");
		else{
			printf("ERROR!ERROR!ERROR!ERROR!ERROR!ERROR!ERROR!\n");
		}

		rv = read_(ITG_ADDRESS, 0x00, 0);
		
		printf("itg WHOAMI response: %d\n",rv);


		// get acceleration on x, y, z axis

		measure_adxl();
		measure_itg();
	

		printf("Accel value: %d\tY value: %d\tZ value: %d\n", accx, accy, accz);

		printf("Gyro value: %d\tY value: %d\tZ value: %d\n", gyrx, gyry, gyrz);

	}
 	PROCESS_END();
}


void init_adxl(){
	write_(ADXL_ADDRESS, 0x1D, 0x40);			
	write_(ADXL_ADDRESS, 0x1E, 0x00);
	write_(ADXL_ADDRESS, 0x1F, 0x00);
	write_(ADXL_ADDRESS, 0x20, 0x00);		
	write_(ADXL_ADDRESS, 0x21, 0x7f);
	write_(ADXL_ADDRESS, 0x22, 0x30);		
	write_(ADXL_ADDRESS, 0x23, 0x7f);
	write_(ADXL_ADDRESS, 0x24, 0x02);		
	write_(ADXL_ADDRESS, 0x25, 0x01);
	write_(ADXL_ADDRESS, 0x26, 0xff);
	write_(ADXL_ADDRESS, 0x27, 0xff);
	write_(ADXL_ADDRESS, 0x28, 0x05);
	write_(ADXL_ADDRESS, 0x29, 0x14);
	write_(ADXL_ADDRESS, 0x2A, 0x07);
	write_(ADXL_ADDRESS, 0x2B, 0x00);
	//write_(ADXL_ADDRESS, 0x2C, 0x0A);//100 Hz sampling
	write_(ADXL_ADDRESS, 0x2C, 0x07);//12.5 Hz sampling
	write_(ADXL_ADDRESS, 0x2D, 0x08);		
	write_(ADXL_ADDRESS, 0x2E, 0x00);		
	write_(ADXL_ADDRESS, 0x31, 0x00);//not full_res, +- 2g
//	write_(ADXL_ADDRESS, 0x31, 0x04);//full_res, +- 2g

}


void init_itg(){
	write_(ITG_ADDRESS, 0x15, 0x07);			
	write_(ITG_ADDRESS, 0x16, 0x19);			
	write_(ITG_ADDRESS, 0x17, 0x00);			
	write_(ITG_ADDRESS, 0x3E, 0x01);			
}


void set_measure(){
	write_(ADXL_ADDRESS, 0x2D, 0x08);
}

void measure_adxl(){ // standard data accessing according to the datasheet

	unsigned char measurevector[6];
	read_multibyte(ADXL_ADDRESS, 0x32, 6, measurevector);
	//xx0 = measurevector[0];
	//xx1 = measurevector[1];
	//yy0 = measurevector[2];
	//yy1 = measurevector[3];
	//zz0 = measurevector[4];
	//zz1 = measurevector[5];


	//append the 2 bytes		
	accx = 0;
	accx |= measurevector[1];
	accx = (accx<<8) | measurevector[0];
	
	accy = 0;
	accy |= measurevector[3];
	accy = (accy<<8) | measurevector[2];
		
	accz = 0;
	accz |= measurevector[5];
	accz = (accz<<8) | measurevector[4];
	
	//printf("X: %d %d \nY: %d %d \nZ: %d %d \n",  xx0, xx1, yy0, yy1, zz0, zz1);
}


void measure_itg(){ 

	unsigned char measurevector[8];
	read_multibyte(ITG_ADDRESS, 0x1B, 8, measurevector);	

	gyrx = 0;
	gyrx |= measurevector[2];
	gyrx = (gyrx<<8) | measurevector[3];
	
	gyry = 0;
	gyry |= measurevector[4];
	gyry = (gyry<<8) | measurevector[5];
		
	gyrz = 0;
	gyrz |= measurevector[6];
	gyrz = (gyrz<<8) | measurevector[7];
	
}

void strange_measure(){ // non_standard reading. accessing each axis separately

/*	xx0 = read_(ADXL_ADDRESS, 0x32, 0);
	xx1 = read_(ADXL_ADDRESS, 0x33, 0);
	yy0 = read_(ADXL_ADDRESS, 0x34, 0);
	yy1 = read_(ADXL_ADDRESS, 0x35, 0);
	zz0 = read_(ADXL_ADDRESS, 0x36, 0);		
	zz1 = read_(ADXL_ADDRESS, 0x36, 0);*/
}


void start(){
	i2c_enable();
	int suc;
	suc = i2c_start();
	while(suc == -1){
		printf("start failed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		suc=i2c_start();
	}
}

void write(unsigned _c){
	if (i2c_write(_c)==0)
		printf("write unsuccessful!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void stop(){
	i2c_stop();
	i2c_disable();
}