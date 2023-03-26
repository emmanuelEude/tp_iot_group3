#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/radio-sensor.h"
#include "dev/sht11.h"
#include <stdio.h>

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"
#include <string.h>

#define SERVER_PORT 3001
#define CLIENT_PORT 3000

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *client_conn;

PROCESS(udp_client_process, "UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);

/*---------------------------------------------------------------------------*/
static void
process_led_message(char *payload)
{
  if (strcmp(payload, "ON") == 0) {
    leds_on(LEDS_GREEN);
  } else if (strcmp(payload, "OFF") == 0) {
    leds_off(LEDS_GREEN);
  } else {
    PRINTF("Invalid LED message received: %s\n", payload);
  }
}

static void
process_radio_message(char *payload)
{
  int16_t value = radio_sensor.value(0);
  char buf[MAX_PAYLOAD_LEN];
  sprintf(buf, "Radio sensor value: %d", value);
  uip_udp_packet_send(client_conn, buf, strlen(buf));
}

static void
process_button_message()
{
  char buf[MAX_PAYLOAD_LEN];
  sprintf(buf, "Button clicked");
  uip_udp_packet_send(client_conn, buf, strlen(buf));
}

static void
process_temperature_message()
{
  int16_t temp = sht11_temp();
  char buf[MAX_PAYLOAD_LEN];
  sprintf(buf, "Temperature sensor value: %d", temp);
  uip_udp_packet_send(client_conn, buf, strlen(buf));
}

static void
process_message(char *payload)
{
  char *type = strtok(payload, " ");
  char *value = strtok(NULL, "");

  if (strcmp(type, "LED") == 0) {
    process_led_message(value);
  } else if (strcmp(type, "RADIO") == 0) {
    process_radio_message(value);
  } else if (strcmp(type, "BUTTON") == 0) {
    process_button_message();
  } else if (strcmp(type, "TEMPERATURE") == 0) {
    process_temperature_message();
  } else {
    PRINTF("Invalid message type received: %s\n", type);
  }
}

static void
tcpip_handler(void)
{
  if(uip_newdata()) {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    PRINTF("Client received: '%s' from server\n", (char *)uip_appdata);

    process_message((char *)uip_appdata);
  }
}

/*—————————————————————————————————————*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  client_conn = udp_new(NULL, UIP_HTONS(SERVER_PORT), NULL);
  udp_bind(client_conn, UIP_HTONS(CLIENT_PORT));

  etimer_set(&timer, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == tcpip_event) {
      tcpip_handler();
    } else if (ev == sensors_event && data == &button_sensor) {
      char buf[MAX_PAYLOAD_LEN];
      sprintf(buf, "BUTTON CLICKED");

      uip_udp_packet_send(client_conn, buf, strlen(buf));
    } else if (ev == PROCESS_EVENT_TIMER) {
      char buf[MAX_PAYLOAD_LEN];
      int16_t value = radio_sensor.value(0);
      sprintf(buf, "RADIO %d", value);

      uip_udp_packet_send(client_conn, buf, strlen(buf));
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}


