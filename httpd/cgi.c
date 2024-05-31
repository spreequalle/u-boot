/**
 * \addtogroup httpd
 * @{
 */

/**
 * \file
 * HTTP server script language C functions file.
 * \author Adam Dunkels <adam@dunkels.com>
 *
 * This file contains functions that are called by the web server
 * scripts. The functions takes one argument, and the return value is
 * interpreted as follows. A zero means that the function did not
 * complete and should be invoked for the next packet as well. A
 * non-zero value indicates that the function has completed and that
 * the web server should move along to the next script line.
 *
 */

/*
 * Copyright (c) 2001, Adam Dunkels.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: cgi.c,v 1.1.1.1 2006/02/08 12:11:56 solo Exp $
 *
 */
#include <common.h>
#include <command.h>
#include "uip.h"
#include "cgi.h"
#include "httpd.h"
#include "fs.h"
#include "header.h"
#include "../include/configs/rt2880.h"
#include "../include/net.h"



#define HWID_LEN 24
extern const char HWID[];

int content_length = 0;

// Predefine function
char *get_request_line(int *request_len, int *request_idx);
u8_t upgrade_cgi(u8_t next);
u8_t upgrade_cgi_2(u8_t next);
u8_t restore_cgi(u8_t next);

cgifunction cgitab[] = {
	upgrade_cgi, /* CGI function "c" */
	upgrade_cgi_2,
	restore_cgi
};

static const char closed[] =   /*  "CLOSED",*/
{0x43, 0x4c, 0x4f, 0x53, 0x45, 0x44, 0};
static const char syn_rcvd[] = /*  "SYN-RCVD",*/
{0x53, 0x59, 0x4e, 0x2d, 0x52, 0x43, 0x56, 
	0x44,  0};
static const char syn_sent[] = /*  "SYN-SENT",*/
{0x53, 0x59, 0x4e, 0x2d, 0x53, 0x45, 0x4e, 
	0x54,  0};
static const char established[] = /*  "ESTABLISHED",*/
{0x45, 0x53, 0x54, 0x41, 0x42, 0x4c, 0x49, 0x53, 0x48, 
	0x45, 0x44, 0};
static const char fin_wait_1[] = /*  "FIN-WAIT-1",*/
{0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49, 
	0x54, 0x2d, 0x31, 0};
static const char fin_wait_2[] = /*  "FIN-WAIT-2",*/
{0x46, 0x49, 0x4e, 0x2d, 0x57, 0x41, 0x49, 
	0x54, 0x2d, 0x32, 0};
static const char closing[] = /*  "CLOSING",*/
{0x43, 0x4c, 0x4f, 0x53, 0x49, 
	0x4e, 0x47, 0};
static const char time_wait[] = /*  "TIME-WAIT,"*/
{0x54, 0x49, 0x4d, 0x45, 0x2d, 0x57, 0x41, 
	0x49, 0x54, 0};
static const char last_ack[] = /*  "LAST-ACK"*/
{0x4c, 0x41, 0x53, 0x54, 0x2d, 0x41, 0x43, 
	0x4b, 0};

unsigned long total_size = 0;
unsigned long total_filesize = 0;
unsigned long total_packets = 0;

#if defined (CFG_ENV_IS_IN_SPI)

#define BUFF_LOCATION CFG_SPINAND_LOAD_ADDR
#define LOADER_LOCATION 0		// from address 0
#define KERNEL_LOCATION (CFG_BOOTLOADER_SIZE + CFG_CONFIG_SIZE + CFG_FACTORY_SIZE) // from address (CFG_BOOTLOADER_SIZE + CFG_CONFIG_SIZE + CFG_FACTORY_SIZE)

#else		// defined (CFG_ENV_IS_IN_SPI)

#define BUFF_LOCATION (0x00000000)	// according to RT3052 SDK SDRAM address
#define LOADER_LOCATION (CFG_FLASH_BASE)
#define EARSE_LOADER_SECTION (CFG_FLASH_BASE+CFG_BOOTLOADER_SIZE-1)
#define KERNEL_LOCATION (CFG_KERN_ADDR)
#define EARSE_KERNEL_SECTION (CFG_KERN_ADDR + (CFG_INIT_SP_OFFSET - CFG_BOOTLOADER_SIZE - CFG_CONFIG_SIZE - CFG_FACTORY_SIZE))

#endif	// defined (CFG_ENV_IS_IN_SPI)
#define BUFFER_SIZE (4*1024*1024)
int buffer_ptr = 0;


#define ntohl(a) ( (((a) & 0xff000000L) >> 24) | \
                   (((a) & 0x00ff0000L) >> 8)  | \
                   (((a) & 0x0000ff00L) << 8)  | \
                   (((a) & 0x000000ffL) << 24) )
                   
#define MAX_MODEL_NAME_LEN    20
#define MAX_SIG_LEN        	30
#define MAX_REG_LEN				4
#define MAX_FW_LEN				12
struct img_hdr_struct{
	unsigned long checksum;	
   unsigned char model[MAX_MODEL_NAME_LEN];
	unsigned char sig[MAX_SIG_LEN];	
	unsigned char partition;	
	unsigned char hdr_len;
	unsigned char rsv1;
	unsigned char rsv2;	
	unsigned long flash_byte_cnt;
	unsigned char fw_reg[MAX_REG_LEN];
	unsigned char fw_ver[MAX_FW_LEN];
};

static struct img_hdr_struct img_hdr;

/*-----------------------------------------------------------------------------------*/
/*  *********************************************************************
 *  cgi_get_flashbuf(bufptr, bufsize)
 *  
 *  Figure out the location and size of the staging buffer.
 *  
 *  Input parameters:
 *	   bufptr - address to return buffer location
 *	   bufsize - address to return buffer size
 ********************************************************************* */


static void cgi_get_flash_buf(u8_t **bufptr, int *bufsize){
	int size = BUFFER_SIZE;

	if (size > 0) {
		*bufptr = (u8_t *) BUFF_LOCATION;
		*bufsize = size;
	}
}


char *get_request_line(int *request_len, int *request_idx){
	int i;
	char c;

	for (i = *request_idx; *request_idx < *request_len; ++(*request_idx)){
		c = uip_appdata[*request_idx];
		if (c == '\n' || c == '\r'){
			uip_appdata[*request_idx] = '\0';
			++(*request_idx);
			if (c == '\r' && *request_idx < *request_len && uip_appdata[*request_idx] == '\n'){
				uip_appdata[*request_idx] = '\0';
				++(*request_idx);
			}
						
			return (char *)&(uip_appdata[i]);
		}
	}
	
	return (char*) 0;
}

u8_t upgrade_cgi(u8_t next){
	u8_t *ptr;
	char *line;
	char *cp;			
	int request_len, request_idx;
	int file_idx;
	int len;
	int i;		
	int find_file = 0;
	int bufsize = 0;
	unsigned long dest;
	unsigned long sum = 0;
	
	cgi_get_flash_buf(&ptr, &bufsize);
	
	if(next == 1){
		return 1;
	}
	
	request_len = uip_datalen();
	request_idx = 0;		
	file_idx = 0;
		
	if (total_size == 0)	{
		while((line = get_request_line(&request_len, &request_idx)) != (char*) 0){						
			if(line[0] == '\0'){					
				if (find_file){			
					break;				
				}else{
					file_idx = request_idx;	// get the length of http headers
				}
			}else if(strncmp(line, "POST /cgi/index", 15) == 0){	//	Get "POST /cgi/index" and reset all
				total_size = 0;
				total_packets = 1;
				total_filesize = 0;
			}else if(strncmp(line, "Content-Length:", 15) == 0){	//	Get Content-Length
				cp = &line[16];
				total_filesize = simple_strtol(cp,NULL,0);						
			}else if(strncmp(line, "-----------------------------", 29) == 0){
				content_length = strlen(line);	
			}else if (strncmp(line, "Content-Type: application/octet-stream", 25) == 0) {	//	Start to receive firmware data	    			    		
	    		/*	deduct the attachment in front of HTTP Data	*/	    																			
				cp = (char *)&uip_appdata[request_idx+2];
												
				if(uip_datalen() != 1446) {
   				total_filesize = total_filesize - (request_idx+2 - file_idx) - content_length - 6;
				} else {
   				total_filesize = total_filesize - (request_idx+2 - file_idx);
				} 
																
				len = uip_datalen() - (request_idx + 2);																														
				total_size = len;
				memcpy(ptr, cp, len);
				buffer_ptr = len;                
            find_file = 1;                     																																
			}
		}			
	}else if(total_size != 0 && strncmp(&uip_appdata[0], "POST /cgi/index", 15) != 0){		
		stop_delay = 1;	// to stop the Wireless LED to flash and udelay
					
		total_packets++;		
		len = request_len;
		total_size += len;																														
		cp = (char *)&uip_appdata[0];		
							
		if(total_size < total_filesize){		
			memcpy((ptr+buffer_ptr), cp, len);
			buffer_ptr += len;		
		}else{		
			memcpy((ptr+buffer_ptr), cp, len);
			buffer_ptr += len;	
			memcpy(&img_hdr, ptr, sizeof(struct img_hdr_struct));	// get image header
									
		#if 0			
			printf("img_hdr.checksum = %lx\r\n", ntohl(img_hdr.checksum));
			printf("img_hdr.model = %s\r\n", img_hdr.model);
			printf("img_hdr.sig = %s\r\n", img_hdr.sig);
			printf("img_hdr.hdr_len = %d\r\n", img_hdr.hdr_len);
			printf("img_hdr.flash_byte_cnt = %ld\r\n", ntohl(img_hdr.flash_byte_cnt));
			printf("img_hdr.fw_reg = %s\r\n", img_hdr.fw_reg);
			printf("img_hdr.fw_ver = %s\r\n", img_hdr.fw_ver);
		#endif
		
			// 1. verify model name
			if(strcmp(img_hdr.model, IMGHDR_MODEL) != 0){				
				printf("Check Model ID Error!!!\n");					
				return 2;
			}	
			
			// 2. verify signature
			if(strcmp(img_hdr.sig, LOADER_SIG) != 0 && strcmp(img_hdr.sig, IMGHDR_SIG) != 0){				
				printf("Check Hardware ID Error!!!\n");					
				return 2;
			}
			
			total_filesize -= sizeof(struct img_hdr_struct);
			ptr += sizeof(struct img_hdr_struct);	// move pointer to the image data
								
			// 3. calculate the image's checksum
			for (i = 0; i < total_filesize; i++){
				sum += *(ptr + i) & 0x000000ff;								
			}
			
		   // 4. verify the image's checksum
			if(sum != ntohl(img_hdr.checksum)){				
				printf("Checksum error!\n");				
				return 2;
			}
			
			/*	Start to copy firmware from buffer to FLASH	*/
			printf("Ready to Upgrade Firmware\n");	
			if (strcmp(img_hdr.sig, LOADER_SIG) == 0) {	// upgrade loader
				dest = (unsigned long)LOADER_LOCATION;			
			}else{		// upgrade kernel
				dest = (unsigned long)KERNEL_LOCATION;				
			}
		
#if defined (CFG_ENV_IS_IN_SPI)		
			return 3;	// tell httpd.c to start writing image to flash
#else		// defined (CFG_ENV_IS_IN_SPI)	
			/*	check if we are copying to Flash	*/			
			if (addr2info(dest) != NULL) {
				printf("Copy to Flash... \n");				
				return 3;
			}			
#endif	// defined (CFG_ENV_IS_IN_SPI)


		}			
	}
	
	return 0;
}

u8_t upgrade_cgi_2(u8_t next){	
	int buf_size;	
	int i;
	int error;
	int flag; 	// 0: loader, 1: kenrel
	u8_t *ptr;
	unsigned long dest, addr;	
		
	cgi_get_flash_buf(&ptr, &buf_size);

	memcpy(&img_hdr, ptr, sizeof(struct img_hdr_struct));	// get image header
	ptr += sizeof(struct img_hdr_struct);	// move pointer to the image data
	addr = ptr;
	
	if (strcmp(img_hdr.sig, LOADER_SIG) == 0) {	// upgrade loader
		dest = (unsigned long)LOADER_LOCATION;		
		printf("upgrade loader\r\n");	
#if defined (CFG_ENV_IS_IN_FLASH)		
		flash_sect_protect(0, LOADER_LOCATION, EARSE_LOADER_SECTION);   // remove boot loader's memory protection
		flash_sect_erase(LOADER_LOCATION, EARSE_LOADER_SECTION);   // erase boot loader memory
#endif		
		flag = 0;
	}else{		// upgrade kernel
		dest = (unsigned long)KERNEL_LOCATION;		
		printf("upgrade kernel\r\n");			

#if defined (CFG_ENV_IS_IN_FLASH)		
		flash_sect_erase(KERNEL_LOCATION, EARSE_KERNEL_SECTION);	// need to erase the kernel memory address before upgrading firmware
#endif		
		flag = 1;
	}
	
#if defined (CFG_ENV_IS_IN_FLASH)				
	if (addr2info(dest) != NULL) {
		error = flash_write((u8_t *)addr, dest, total_filesize);		// write data to flash
	
		if (error < 0){
			flash_perror(error);
			error = flash_write((u8_t *)addr, dest, total_filesize);		// write data to flash again
		}else{
			printf("done.\n");
		}
	}
#elif defined (CFG_ENV_IS_IN_SPI)
	if (flag == 0){	// upgrade bootloader
		error = raspi_erase_write(addr, 0, total_filesize);
	}else{
		error = raspi_erase_write(addr, dest, total_filesize);	// wirte image
	}	
#endif

	if(error == 0){
		printf("Finish Upgrade\n");	
		if (flag == 0){
#if defined (CFG_ENV_IS_IN_FLASH)			
			 flash_sect_protect(1, LOADER_LOCATION, EARSE_LOADER_SECTION);   // set boot loader's memory protection
#endif			 
		}
			
		finishflag = 1;	//	Finish Flag	
				
		/*	reset all	*/
		total_size = 0;
		total_filesize = 0;
		total_packets = 0;				
	}else{
		finishflag = 0;
	}
}
/*-----------------------------------------------------------------------------------*/
u8_t restore_cgi(u8_t next)
{
//	restore_default();
	printf("Finish Restore Default\n");
	
	/*	Finish Flag	*/
	finishflag = 1;
	/*	reset all	*/
	total_size = 0;
	total_filesize = 0;
	total_packets = 0;
}
