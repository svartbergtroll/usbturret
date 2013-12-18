/**
 * \file main.c
 * \author Thomas Maurice, Benoit Maliar
 * 
 * \brief Management program for the USB turret
 * \version 0.1
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <libusb-1.0/libusb.h>

#include <turret.h>

/** Serial port used by the program */
#define SERIAL_PORT "/dev/ttyACM0"

/**
 * \brief main, lol
 */
int main(int argc, char ** argv) {
	printf("Initializing the library...\n");
	libusb_context *context = newContext();
	
	int currentDevice = 0; //!< Current USB turret enumerated
	libusb_device_handle *handles[4]; //!< Handle array, shall contain all the four turrets (Like "Gilles")
	//!< -> (gilles of the turret, lol)
	for(int i = 0; i < 4; i++) handles[i] = NULL; // Init to NULL just in case we have < 4 devices
	
	libusb_device **list; //!< List of USB devices connected
	size_t count = libusb_get_device_list(context, &list); // fills in device list
	if(count<0) {
		perror("libusb_get_device_list");
		libusb_exit(context); // Clean exit
		exit(-1);
	}
	
	for(size_t i = 0; i < count; i++){
		libusb_device *device = list[i];
		struct libusb_config_descriptor *configDescriptor;
		struct libusb_device_descriptor desc;
		int status = libusb_get_device_descriptor(device,&desc); // 0 on success
		if(status != 0)
			continue;
		
		uint8_t address = libusb_get_device_address(device);
		
		if(desc.idVendor == VENDOR_TURRET && desc.idProduct == PRODUCT_TURRET) {
			/*status = libusb_get_active_config_descriptor(device, &configDescriptor);
			if(status != 0) {
				libusb_exit(context); // Clean exit
				perror("libusb_get_active_config_descriptor");
				exit(-1);
			}
			
			printf("\t-> Turret detected ... ");
			status = libusb_open(device, &handles[currentDevice]);
			if(status == 0) {
				printf("[HANDLED]\n");
			} else {
				printf("[FAILED]\n");
				perror("libusb_open");
				libusb_exit(context); // Clean exit
				exit(-1);
			}
			
			for(int i = 0; i < configDescriptor->bNumInterfaces; i++) {
				if(libusb_kernel_driver_active(handles[currentDevice], configDescriptor->interface[i].altsetting[0].bInterfaceNumber)) {
					status = libusb_detach_kernel_driver(handles[currentDevice], configDescriptor->interface[i].altsetting[0].bInterfaceNumber);
					if(status != 0){
						libusb_exit(context); // Clean exit
						perror("libusb_detach_kernel_driver");
						exit(-1);
					} else {
						printf("\t* Kernel has released interface %d \\o/\n", i);
					}
				}
			}
			
			libusb_free_config_descriptor(configDescriptor);
			status = libusb_get_config_descriptor(device, 0, &configDescriptor);
			if(status != 0) {
				libusb_exit(context); // Clean exit
				perror("libusb_get_config_descriptor");
				exit(-1);
			}
			
			if(libusb_set_configuration(handles[currentDevice], configDescriptor->bConfigurationValue) == 0) {
				printf("\tConfiguration 0 applied\n");
			} else {
				libusb_exit(context); // Clean exit
				perror("libusb_set_configuration");
				exit(-1);
			}
			
			for(int i = 0; i < configDescriptor->bNumInterfaces; i++) {
				if(libusb_claim_interface(handles[currentDevice], configDescriptor->interface[i].altsetting[0].bInterfaceNumber) == 0) {
					printf("\tInterface %d successfuly claimed !\n", i);
				} else {
					//libusb_exit(context); // Clean exit
					perror("libusb_claim_interface");
					//exit(-1);
				}
			}*/
			
			configureTurret(device, &handles[currentDevice]);
			
			char c = 0;
			
			while(1) {
				c = getc(stdin);
				if(c == 'a') break;
				switch(c) {
					case 'z': send_command(handles[currentDevice], CMD_UP); break;
					case 's': send_command(handles[currentDevice], CMD_DOWN); break;
					case 'd': send_command(handles[currentDevice], CMD_RIGHT); break;
					case 'q': send_command(handles[currentDevice], CMD_LEFT); break;
					case 'e': send_command(handles[currentDevice], CMD_FIRE); break;
					default: break;
				}
			}
			
			releaseHandle(handles[currentDevice]);
			
			currentDevice++;
		}
	}

	libusb_free_device_list(list,1);
	libusb_exit(context);
	
	return EXIT_SUCCESS;
}
