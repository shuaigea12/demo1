#include <stdio.h>
#include <usb.h>
#include <libusb-1.0/libusb.h>
#include <string.h>


#define VENDOR_ID  0x1314
#define PRODUCT_ID 0x1003

int main(void) {

    struct usb_bus *bus;
    struct usb_device *dev,*devt;
    usb_dev_handle *udev;
    int ret;

    devt=NULL;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (bus = usb_busses; bus; bus = bus->next)
    {
      for (dev = bus->devices; dev; dev = dev->next)
        {
            if(VENDOR_ID == dev->descriptor.idVendor)
                  if(PRODUCT_ID == dev->descriptor.idProduct)
                    {
                        printf("\n*****找到了指定的设备! vid/pid:%04X/%04X\n\n",dev->descriptor.idVendor, dev->descriptor.idProduct);

                        devt=dev;

                        break;

                    }

        }
    }

    if(!devt)
    {
        printf("\n*****找不到指定的设备!\n\n");
        return 0;
    }

    udev = usb_open(devt);
    usb_set_configuration(udev,1);

    if(!udev)
    {

        perror("Cannot open device");
    }

    else
    {



        char name[512];
        if (usb_get_driver_np(udev, 0, (char *) name, sizeof(name)) == 0)
        {

            printf("%s\n",name);
            int jk = usb_detach_kernel_driver_np(udev,0);
            if(jk < 0)
            {
                perror("can not detach");
            }

        }
        if(0 != usb_set_configuration(udev,1))
        {
            perror("config error");
        }

        if(0 != usb_claim_interface(udev, 0))
        {
            perror("Cannot claim interface");
            usb_close(udev);
            udev = NULL;
            return 0;

        }


        unsigned char wbuf[21];
        unsigned char rbuf[21];
        int t;
        memset(rbuf,0,21);
        memset(wbuf,0,21);
        wbuf[0]=0x02;wbuf[1]=0x10;wbuf[2]=0xEE;wbuf[3]=0xCC;wbuf[4]=0xF0;wbuf[17]=0xD2;

        //write
        ret = usb_interrupt_write(udev,0x02,(char *)wbuf,sizeof(wbuf),3000);
        if(ret < 0)
        {
            perror("can not write!");
        }
        else
        {
            printf("*****成功写入%d字节:\n",ret);
             for(t=0;t<=20;t++)
            printf("%02X ",wbuf[t]);
            printf("\n\n");


        }

        //read
        ret = usb_interrupt_read(udev,0x82,(char *)rbuf,sizeof(rbuf),3000);
        if(ret < 0)
        {
            perror("can not read!");
        }
        else
        {
            printf("*****成功读入%d字节:\n",ret);
            for(t=0;t<=20;t++)
            printf("%02X ",rbuf[t]);
            printf("\n\n");
        }


        //free
        ret = usb_release_interface(udev,0);
        if(ret<0)
        {
            perror("can not release");
        }

        usb_reset(udev);
        ret = usb_close(udev);
        if(ret < 0)
        {
            perror("can not close");
        }

        udev = NULL;

    }


    return 1;



}
