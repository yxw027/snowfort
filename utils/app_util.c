/*
 * app_util.c
 *
 *  Created on: Aug 3, 2013
 *      Author: yzliao
 */

#include "app_util.h"
#include <stdio.h>
//#include "platform-conf.h"


/*
 * Function: app_output
 * This function prints the payload data to stdout. The printing format is consistent
 * with the read_usb.py script. It is recommended for using this function to print
 * output instead of using the self-developed function.
 */
void app_output(const uint8_t * data, const uint8_t node_id, const uint8_t pkt_seq, const uint8_t payload_len)
{
  int i;

  printf("%u,%u,",node_id,pkt_seq);

	for(i = 0; i < payload_len; i++)
	{
	  printf("%02x",data[i]);
	}
	printf("\n");
}

