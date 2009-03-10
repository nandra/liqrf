#include <stdio.h>
#include <usb.h>

// print device descriptor
void printDevDesc(struct usb_device *dev)
{
	printf("bDescriptorType: %d (0x%x)\n", dev->descriptor.bDescriptorType, dev->descriptor.bDescriptorType);
	printf("bcdUSB: %d (0x%x)\n", dev->descriptor.bcdUSB, dev->descriptor.bcdUSB);
	//printf("bDeviceSubclass: %d (0x%x)\n", dev->descriptor.bDeviceSubclass, dev->descriptor.bDeviceSubclass);
	printf("bDeviceProtocol: %d (0x%x)\n", dev->descriptor.bDeviceProtocol, dev->descriptor.bDeviceProtocol);
	printf("idVendor: %d (0x%x)\n", dev->descriptor.idVendor, dev->descriptor.idVendor);
	printf("idProduct: %d (0x%x)\n", dev->descriptor.idProduct, dev->descriptor.idProduct);
	printf("bcdDevice: %d (0x%x)\n", dev->descriptor.bcdDevice, dev->descriptor.bcdDevice);
	printf("iManufacturer: %d (0x%x)\n", dev->descriptor.iManufacturer, dev->descriptor.iManufacturer);
	printf("iProduct: %d (0x%x)\n", dev->descriptor.iProduct, dev->descriptor.iProduct);
}

// print configuration descriptor
void printConfDesc()
{
	;
}

main()
{
	struct usb_bus *busses;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();
	printf("usb program\n");

	struct usb_bus *bus;
	int c, i, a;
	
	for(bus=busses; bus; bus=bus->next)
	{
		struct usb_device *dev;

		for(dev=bus->devices; dev; dev=dev->next)
		{
			switch(dev->descriptor.bDeviceClass)
			{
				case 0:
					printf("Device: class %d: base class \n", dev->descriptor.bDeviceClass);
					int inter = 0;

					printDevDesc(dev);

					// loop through all of the configurations
					for(c=0; c < dev->descriptor.bNumConfigurations; c++)
					{
						// loop through all of the interfaces
						for(i=0; i < dev->config[c].bNumInterfaces; i++)
						{
							printf("Interface %d\n", i);
							
						}
						inter += i+1;
					}

					printf("	- %d configurations\n", c+1);
					printf("	- %d interfaces\n", inter);
					break;

				case 2:
					printf("Device: class %d: communications \n", dev->descriptor.bDeviceClass);
					break;

				case 9:
					printf("Device: class %d: hub \n", dev->descriptor.bDeviceClass);
					break;

				case 239:
					printf("Device: class %d: miscellaneous \n", dev->descriptor.bDeviceClass);
					break;

				default:
					printf("Device: class %d: unknown \n", dev->descriptor.bDeviceClass);
					break;
			}
		}
	}
} 
