#include <stdio.h>
#include "contiki.h"
#include "net/packetbuf.h"

#define PRINTADDR(addr) printf(" %02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7])

PROCESS(null_app_process, "Null App Process");
AUTOSTART_PROCESSES(&null_app_process);

PROCESS_THREAD(null_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("Null App Started\n");
  PRINTADDR(&rimeaddr_node_addr);

  PROCESS_END();
}