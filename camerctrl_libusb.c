#include <stdio.h>
#include <stdlib.h>
#include <usb.h>

typedef struct
{
	struct usb_device* udev;
	usb_dev_handle* device_handle;
}device_descript;

#define USB_TIMEOUT		3000
#define VENDOR_ID		0x1292
#define PRODUCT_ID		0x4453
//#define VENDOR_ID		0x1e3d
//#define PRODUCT_ID		0x2093
#define DEVICE_MINOR	16

int g_num;
device_descript g_list[DEVICE_MINOR];

int Device_Find()
{
	struct usb_bus*		bus;
	struct usb_device*	dev;
	g_num =0;
	usb_find_busses();
	usb_find_devices();

	for(bus = usb_busses;bus;bus = bus->next)
	{
		for(dev = bus->devices;dev;dev = dev->next)
		{
			if(dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID)
			{
			printf("find is ok!\n");
			g_list[g_num].udev = dev;
			return g_num;
			}
//			printf("vendor:%x,product:%x\n",dev->descriptor.idVendor,dev->descriptor.idProduct);
		}
	}
	return g_num;
}

int Device_Open()
{
	if(g_list[g_num].udev !=NULL)
	{
		g_list[g_num].device_handle = usb_open(g_list[g_num].udev);
		if(g_list[g_num].device_handle ==NULL)
		{
			printf("Open Usb Device failed\n");
			return -1;
		}
		printf("Device_Open Usb Device successful!!\n");
		return 0;
	}
	else
	{
		printf("NO Device_Open!!\n");
		return -1;
	}
}

int Device_Write_Ready()
{
	int ret =0;
	if(Device_Open()!=-1)
	{
//		ret =usb_detach_kernel_driver_np(g_list[g_num].device_handle,0);
		if(ret <0)
		{
			printf("ret:%d\n",ret);
			perror("usb_detach_kernel_driver_np");
			usb_close(g_list[g_num].device_handle);
			return ret;
		}
		ret =usb_claim_interface(g_list[g_num].device_handle,0);
		if(ret <0)
		{
			printf("ret:%d\n",ret);
			perror("usb_claim_interface");
			usb_close(g_list[g_num].device_handle);
			return ret;
		}
			printf("Device_Write_Ready!!\n");
		return ret;
	}
	else
		return -1;
}

int Device_Write(unsigned char *buffer)
{
	int send_len =8;
	send_len = usb_control_msg(g_list[g_num].device_handle,USB_ENDPOINT_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,USB_REQ_SET_CONFIGURATION,0x300,2,buffer,8,USB_TIMEOUT);
	if(send_len <=0)
	{
		printf("send_len:%d\n",send_len);
		perror("usb_control_msg");
	}
	if(send_len != 8)
		return send_len;
}

void Device_close()
{
	usb_release_interface(g_list[g_num].device_handle,0);
	usb_close(g_list[g_num].device_handle);
}

int main()
{
	unsigned char send_data[8]={0};
	send_data[0] = 0xa0;
	send_data[1] = 0x20;
	send_data[2] = 0x05;
	send_data[3] = 0x05;
	send_data[4] = 0x05;
	send_data[5] = 0x05;
	send_data[6] = 0x05;
	send_data[7] = 0x05;
	usb_init();
	Device_Find();
	if(Device_Write_Ready() >= 0)
	{
		Device_Write(send_data);
		Device_close();
	}
	return 0;
}
