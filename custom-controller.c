#include "contiki.h"
#include "contiki-net.h"
#include "dev/light.h"
#include "dev/radio-sensor.h"
#include "dev/button-sensor.h"
#include "dev/sht11.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT_LED 3000
#define UDP_PORT_RADIO 3001
#define UDP_PORT_BUTTON 3002
#define UDP_PORT_TEMP 3003
#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *udp_conn_led;
static struct uip_udp_conn *udp_conn_radio;
static struct uip_udp_conn *udp_conn_button;
static struct uip_udp_conn *udp_conn_temp;

PROCESS(udp_server_process, "UDP server process");
AUTOSTART_PROCESSES(&udp_server_process);

PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  udp_conn_led = udp_new(NULL, UIP_HTONS(UDP_PORT_LED), NULL);
  udp_bind(udp_conn_led, UIP_HTONS(UDP_PORT_LED));

  udp_conn_radio = udp_new(NULL, UIP_HTONS(UDP_PORT_RADIO), NULL);
  udp_bind(udp_conn_radio, UIP_HTONS(UDP_PORT_RADIO));

  udp_conn_button = udp_new(NULL, UIP_HTONS(UDP_PORT_BUTTON), NULL);
  udp_bind(udp_conn_button, UIP_HTONS(UDP_PORT_BUTTON));

  udp_conn_temp = udp_new(NULL, UIP_HTONS(UDP_PORT_TEMP), NULL);
  udp_bind(udp_conn_temp, UIP_HTONS(UDP_PORT_TEMP));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    } else if(ev == PROCESS_EVENT_TIMER) {
      // Envoyer des données au serveur
      char  buf[MAX_PAYLOAD_LEN];
      int len;

      // Vérifier si le message provient de la LED
      if(uip_newdata() && uip_udp_conn == udp_conn_led) {
        char *ptr = (char *)uip_appdata;
        if (strncmp(ptr, "LED:ON", 6) == 0) {
          printf("Allumer la LED\n");
          // Allumer la LED
          light_on(0);
          // Envoyer un message de confirmation
          len = snprintf(buf, sizeof(buf), "LED:GREEN");
          uip_udp_packet_send(udp_conn_led, buf, len);
        } else if (strncmp(ptr, "LED:OFF", 7) == 0) {
          printf("Eteindre la LED\n");
          // Eteindre la LED
          light_off(0);
          // Envoyer un message de confirmation
          len = snprintf(buf, sizeof(buf), "LED:RED");
          uip_udp_packet_send(udp_conn_led, buf, len);
        }
      }

      // Vérifier si le message provient de la radio
      if(uip_newdata() && uip_udp_conn == udp_conn_radio) {
        printf("Radio: %d\n", radio_sensor.value(0));
        // Envoyer la valeur courante de la radio
        len = snprintf(buf, sizeof(buf), "RADIO:%d", radio_sensor.value(0));
        uip_udp_packet_send(udp_conn_radio, buf, len);
      }

        // Vérifier si le message provient du bouton
      if(uip_newdata() && uip_udp_conn == udp_conn_button) {
        printf("Bouton cliqué\n");
        // Envoyer un message de confirmation
        len = snprintf(buf, sizeof(buf), "BUTTON:CLICKED");
        uip_udp_packet_send(udp_conn_button, buf, len);
      }

      // Vérifier si le message provient du capteur de température
      if(uip_newdata() && uip_udp_conn == udp_conn_temp) {
        printf("Temperature: %d\n", ((sht11_sensor.value(SHT11_SENSOR_TEMP)/10)-396)/10.0);
        // Envoyer la valeur actuelle du capteur de température
        len = snprintf(buf, sizeof(buf), "TEMP:%d", ((sht11_sensor.value(SHT11_SENSOR_TEMP)/10)-396)/10.0);
        uip_udp_packet_send(udp_conn_temp, buf, len);
      }
    }
  }

  PROCESS_END();
}