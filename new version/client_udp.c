#include "contiki-lib.h"
#include "contiki-net.h"
#include <stdio.h>
#include <string.h>
#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define SERVER_IP_ADDR "aaaa::1"

#define LOCAL_PORT 3001
#define REMOTE_PORT 3000

static struct uip_udp_conn *udp_conn;

PROCESS(udp_client_process, "UDP client process");

AUTOSTART_PROCESSES(&udp_client_process);

PROCESS_THREAD(udp_client_process, ev, data) { PROCESS_BEGIN();

static char buf[100]; uip_ip6addr_t server_ipaddr;

// Parse server IP address
 if(uiplib_ip6addrconv(SERVER_IP_ADDR, &server_ipaddr) == 0) {
	 printf("Invalid server address\n"); 
	PROCESS_EXIT();
 }

// Open a UDP connection 	
	udp_conn = udp_new(&server_ipaddr, UIP_HTONS(REMOTE_PORT), NULL); 
	udp_bind(udp_conn, UIP_HTONS(LOCAL_PORT));

// Send a message to the server
	 strcpy(buf, "Hello, server!"); 
	printf("Sending message: %s\n", buf); 
	uip_udp_packet_send(udp_conn, buf, strlen(buf));

// Wait for a response from the server 
 while(1) { 
	PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);

	if(uip_newdata()) {
	  printf("Received response: %s\n", uip_appdata);
	  break;
	}



 }

// Close the connection udp_remove(udp_conn);

  PROCESS_END(); 
}
