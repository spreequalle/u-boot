/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Boot support
 */
#include <common.h>	// for u32 and cpu_to_le32
#include <command.h>
#include <rt_mmap.h>
#include <watchdog.h>
#include "http.h"
#include <net.h>
#include "../httpd/uip.h"
#include "../httpd/uip_arp.h"
#include "../httpd/httpd.h"
#include "../httpd/uipopt.h"
#include "../drivers/rt2880_eth.h"

extern struct eth_device* 	rt2880_pdev;

/*
 * Address of RALINK_ Registers
 */
#define RALINK_REG_GPIOMODE	(RALINK_SYSCTL_BASE + 0x60)

#define RALINK_PRGIO_ADDR		RALINK_PIO_BASE // Programmable I/O
#define RALINK_REG_PIODATA		(RALINK_PRGIO_ADDR + 0x20)
#define RALINK_REG_PIODIR		(RALINK_PRGIO_ADDR + 0x24)

#define RALINK_GPIO_DIR_ALLIN		0
#define RALINK_GPIOMODE_UARTF		0x1C
#define RALINK_GPIO_8				0x00000100
#define RALINK_GPIO_9				0x00000200

#define WLAN_LED						(RALINK_11N_MAC_BASE + 0x102c)

#define ETH_HEADER ((struct uip_eth_hdr *)&uip_buf[0])
int finishflag = 0;
int stop_delay = 0;	// to indicate when we should stop flashing the security led and udelay. Otherwise, uploading firmware will be very slow

int HttpStart(){	
   u8_t arptimer;
	int i, ret;
	unsigned long gpio_mode = 0;
	unsigned long io_data = 0;
	
	DECLARE_GLOBAL_DATA_PTR;	
	bd_t bd;
	
	memcpy(&bd, gd->bd, sizeof(bd_t));
	
	// uip init
	printf("uip init...\r\n");
	uip_init();

	printf("http init...\r\n");
	httpd_init();
	
	//Set IP address and MAC address
	uip_hostaddr[0] = (htons((bd.bi_ip_addr & 0x0000ffff)));	// save the 2 bytes(MSB) of ip address
	uip_hostaddr[1] = (htons((bd.bi_ip_addr >> 16) & 0xffff));	// save the 2 bytes(LSB) of ip address
		
	for(i = 0; i < 6; i++){	// don't use bd.bi_enetaddr, due to Memory Alignment
		uip_ethaddr[i] = rt2880_pdev->enetaddr[i];
	}
	
	uip_arp_netmask[0] = 0xffff;
	uip_arp_netmask[1] = htons(0xff00);

	arptimer = 0;
	
	//config these pins to gpio mode
	gpio_mode = le32_to_cpu(*(volatile u32 *)(RALINK_REG_GPIOMODE));
	gpio_mode |= RALINK_GPIOMODE_UARTF;
	*(volatile u32 *)(RALINK_REG_GPIOMODE) = cpu_to_le32(gpio_mode);

	gpio_mode = RALINK_GPIO_DIR_ALLIN | RALINK_GPIO_8 | RALINK_GPIO_9;			
	*(volatile u32 *)(RALINK_REG_PIODIR) =	cpu_to_le32(gpio_mode);	
	io_data = RALINK_GPIO_8;
		
	while(1) {	
		uip_len = 0;
		
		*(volatile u32 *)(RALINK_REG_PIODATA) = cpu_to_le32(io_data | ~RALINK_GPIO_9);	// turn on/off Power LED
		
		if (stop_delay == 0){																			
			io_data = ~io_data;
		}else{
			io_data = RALINK_GPIO_8;	// make the power led turn on in color red consistently when doing upgrade firmware
		}
		
		ret = rt2880_eth_recv(rt2880_pdev);
		
		// Abort if ctrl-c was pressed.
		if (ctrlc()) {
			eth_halt();
			printf("\nAbort\n");
			break;
		}
		
		if(finishflag) {
			/* Reset system */
			printf("System Reset \n");
			break;
		}
						
		if(uip_len == 0){				
			// Call the ARP timer function every 10 seconds. 
			if(++arptimer == 20){
				uip_arp_timer();
				arptimer = 0;
			}						
		}else{		
		
//			
//			if (ETH_HEADER->type == htons(UIP_ETHTYPE_IP)){ // UIP_ETHTYPE_IP												    		
//				for (i = 0; i < uip_len; i++){
//					printf("%02x ", uip_buf[i]);
//					
//					if ((i > 0) && (i % 16) == 0){
//						printf("\r\n");
//					}
//				}	
//				printf("\r\n");						
//				uip_arp_ipin();						
//				uip_input();
//				
//				/* If the above function invocation resulted in data that
//				* should be sent out on the network, the global variable
//				* uip_len is set to a value > 0. */
//				if(uip_len > 0){					
//					uip_arp_out();				
//					NetSendPacket(uip_buf, uip_len);					
//				}			
//			}else if (ETH_HEADER->type == htons(UIP_ETHTYPE_ARP)){	// UIP_ETHTYPE_ARP								
//				uip_arp_arpin();
//				/* If the above function invocation resulted in data that
//				* should be sent out on the network, the global variable
//				* uip_len is set to a value > 0. */	
//				if(uip_len > 0) {	
//					NetSendPacket(uip_buf, uip_len);
//				}
//			}
		}			
		
		if (stop_delay == 0){				
			udelay(500000);
		}							
	}
	
	if (finishflag){
		do_reset(NULL, 0, 0, NULL);
	}
	
	return 0;
}
