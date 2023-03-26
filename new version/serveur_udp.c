#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "dev/leds.h"
#include "dev/radio-sensor.h"
#include "dev/button-sensor.h"
#include "dev/sht11.h"

#include <stdio.h>
#include <string.h>
#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define UDP_PORT 3000

static struct uip_udp_conn *udp_conn;

PROCESS(udp_process, "UDP process");
AUTOSTART_PROCESSES(&udp_process);

/*---------------------------------------------------------------------------*/
/* Fonction pour allumer ou éteindre la LED */
void set_led_state(char *state) {
  if(strcmp(state, "ON") == 0) {
    leds_on(LEDS_GREEN);
  } else {
    leds_on(LEDS_RED);
  }
}

/*---------------------------------------------------------------------------*/
/* Fonction pour envoyer un datagramme UDP */
void send_message(char *message, int len) {
  uip_udp_packet_send(udp_conn, message, len);
}

/*---------------------------------------------------------------------------*/
/* Fonction pour traiter les messages entrants */
void handle_message(char *message, int len) {
	printf("TEST");
  if(strncmp(message, "LED:", 4) == 0) {
    char *state = message + 4;
    set_led_state(state);
  } else if(strncmp(message, "RADIO", 5) == 0) {
    char value[10];
    snprintf(value, sizeof(value), "%d", radio_sensor.value(0));
    send_message(value, strlen(value));
  } else if(strncmp(message, "BUTTON", 6) == 0) {
    send_message("bouton cliqué", 14);
  } else if(strncmp(message, "TEMP", 4) == 0) {
    char value[10];
    int16_t temp = sht11_temp();
    snprintf(value, sizeof(value), "%d", temp);
    send_message(value, strlen(value));
  }
}

/* Fonction pour recevoir un datagramme UDP */
int udp_recv(struct uip_udp_conn *c, void *ptr, int maxlen) { 
	int len = 0; 
	if(uip_newdata()) {
		 len = uip_datalen(); 
		if(len > maxlen) { 
			len = maxlen; 
		} 
		memcpy(ptr, uip_appdata, len);
	 } 
	return len;
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_process, ev, data)
{
  PROCESS_BEGIN();

  udp_conn = udp_new(NULL, UIP_HTONS(3001), NULL);
  udp_bind(udp_conn, UIP_HTONS(3000));

  printf("Listening on port %u\n", UDP_PORT);

  while(1) {
   PROCESS_YIELD();
    if(ev == tcpip_event) {
	 printf("Received message ...");
	    static char buf[100];
	    int len = udp_recv(udp_conn, buf, sizeof(buf));
	    if(len > 0) {
	      buf[len] = '\0';
	      printf("Received message: %s\n", buf);
	      handle_message(buf, len);
	    }
	    PROCESS_PAUSE();
   }
  }

  PROCESS_END();
}
