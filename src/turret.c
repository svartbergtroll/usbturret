/**
 * \file turret.c
 * \author Thomas Maurice
 * 
 * \brief Turret handling functions
 * \version 0.1
 */
 
#include <turret.h>

/**
 * Send an order to the turret. This is a *step* order, this is to say
 * that if you want the turret to go up, then it will go up for 300ms
 * in order to make a short move.
 * 
 * \param handle USB handle for the device
 * \param cmd Command to send, must be one of the commands enum constants
 */
void send_command(libusb_device_handle* handle, int cmd) {
	char request1[] = {'U', 'S', 'B', 'C', 0x00, 0x00, 0x04, 0x00};
	char request2[] = {'U', 'S', 'B', 'C', 0x00, 0x40, 0x02, 0x00};
	char stop[64] = {0x0};
	char req[64] = {0x0};
	
	req[6] = 0x08; req[7] = 0x08;
	stop[6] = 0x08; stop[7] = 0x08;
	
	switch (cmd) {
		case CMD_UP: req[3] = 0x01; break;
		case CMD_DOWN: req[4] = 0x01; break;
		case CMD_LEFT: req[1] = 0x01; break;
		case CMD_RIGHT: req[2] = 0x01; break;
		case CMD_FIRE: req[5] = 0x01; break;
		default: break;
	}
	
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, request1, 8, 100);
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, request2, 8, 100);
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, req, 64, 100);
	
	usleep(300000);
	
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, request1, 8, 100);
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, request2, 8, 100);
	libusb_control_transfer(handle, 0x21, 0x09, 0x02, 0x01, stop, 64, 100);
	
	return;
}

/**
 * This functions returns a new libusb context
 * \return A new libusb_context*
 */
libusb_context* newContext() {
	libusb_context *context;
	int status=libusb_init(&context);
	if(status!=0) {
		perror("libusb_init");
		exit(-1);
	}
	
	return context;
}

/**
 * This will configure a new turret via the **handle**
 * parameter. This should not be initialized prior to the call.
 * You are responsible for freeing all the allocated memory.
 * 
 * \param device USB device to configure
 * \param handle Pointer on a libusb_device_handle* to initialize
 */
void configureTurret(libusb_device *device, libusb_device_handle** handle) {
	struct libusb_config_descriptor *configDescriptor;
	int status = libusb_get_active_config_descriptor(device, &configDescriptor);
	if(status != 0) {
		perror("libusb_get_active_config_descriptor");
		exit(-1);
	}
	
	printf("\t-> Turret detected ... ");
	status = libusb_open(device, handle);
	if(status == 0) {
		printf("[HANDLED]\n");
	} else {
		printf("[FAILED]\n");
		perror("libusb_open");
		exit(-1);
	}
	
	for(int i = 0; i < configDescriptor->bNumInterfaces; i++) {
		if(libusb_kernel_driver_active(*handle, configDescriptor->interface[i].altsetting[0].bInterfaceNumber)) {
			status = libusb_detach_kernel_driver(*handle, configDescriptor->interface[i].altsetting[0].bInterfaceNumber);
			if(status != 0){
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
		perror("libusb_get_config_descriptor");
		exit(-1);
	}
	
	if(libusb_set_configuration(*handle, configDescriptor->bConfigurationValue) == 0) {
		printf("\tConfiguration 0 applied\n");
	} else {
		perror("libusb_set_configuration");
		exit(-1);
	}
	
	for(int i = 0; i < configDescriptor->bNumInterfaces; i++) {
		if(libusb_claim_interface(*handle, configDescriptor->interface[i].altsetting[0].bInterfaceNumber) == 0) {
			printf("\tInterface %d successfuly claimed !\n", i);
		} else {
			//libusb_exit(context); // Clean exit
			perror("libusb_claim_interface");
			//exit(-1);
		}
	}
}

/** 
 * This will reset the turret's config and free all the
 * allocated memory.
 * 
 * \param handle Device handle to free.
 */
int releaseHandle(libusb_device_handle* handle) {
	struct libusb_config_descriptor *configDescriptor;
	libusb_device *device = libusb_get_device(handle);
	int status = libusb_get_active_config_descriptor(device, &configDescriptor);
	if(status != 0) {
		perror("libusb_get_active_config_descriptor");
		return -1;
	}
	
	for(int i = 0; i < configDescriptor->bNumInterfaces; i++) {
		if(libusb_release_interface(handle, configDescriptor->interface[i].altsetting[0].bInterfaceNumber) == 0) {
			printf("\tInterface %d successfuly released !\n", i);
		} else {
			perror("libusb_claim_interface");
			return -1;
		}
	}
	
	libusb_close(handle);
	
	return 0;
}

