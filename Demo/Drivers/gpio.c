/**
 *	Quick and very Dirty GPIO API.
 *
 **/

#include "gpio.h"

#define BANK(pin)	((pin) >> 5)
#define MASK(pin)	(1UL << ((pin) & 0x1F))

// Base memory location for GPIO registers:
#define GPIO_REG_BASE	0x20200000

// Define a GPIO register:
#define GPIO_REG(offs)	((volatile unsigned long *) (GPIO_REG_BASE + offs))

// Function selection:
#define GPFSEL		GPIO_REG (0x0000)

// Set and clear pins:
#define GPSET		GPIO_REG (0x001C)
#define GPCLR		GPIO_REG (0x0028)

// Pin level readout:
#define GPLEV		GPIO_REG (0x0034)

// Pin event detection status:
#define GPEDS		GPIO_REG (0x0040)

// Pin event detection:
#define GPREN		GPIO_REG (0x004C)
#define GPFEN		GPIO_REG (0x0058)
#define GPHEN		GPIO_REG (0x0064)
#define GPLEN		GPIO_REG (0x0070)
#define GPAREN		GPIO_REG (0x007C)
#define GPAFEN		GPIO_REG (0x0088)

void
gpioFunctionSet (const unsigned int pin, const unsigned int func)
{
	int offset = pin / 10;

	// Read in the original register value:
	unsigned long val = GPFSEL[offset];

	int item = pin % 10;
	val &= ~(0x7 << (item * 3));
	val |= ((func & 0x7) << (item * 3));
	GPFSEL[offset] = val;
}

void
gpioDirectionSet (const unsigned int pin, const enum GpioDir dir)
{
	gpioFunctionSet(pin, dir);
}

void
gpioWrite (const unsigned int pin, const unsigned int val)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	val ? (GPSET[bank] |= mask)
	    : (GPCLR[bank] |= mask);
}

unsigned int
gpioRead (const unsigned int pin)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	return (GPLEV[bank] & mask) ? 1 : 0;
}

static volatile unsigned long *detect_map[] = {
	[GPIO_DETECT_RISING]        = GPREN,
	[GPIO_DETECT_FALLING]       = GPFEN,
	[GPIO_DETECT_HIGH]          = GPHEN,
	[GPIO_DETECT_LOW]           = GPLEN,
	[GPIO_DETECT_RISING_ASYNC]  = GPAREN,
	[GPIO_DETECT_FALLING_ASYNC] = GPAFEN,
};

void
gpioDetectEnable (const unsigned int pin, const enum GpioDetect type)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	detect_map[type][bank] |= mask;
}

void
gpioDetectDisable (const unsigned int pin, const enum GpioDetect type)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	detect_map[type][bank] &= ~mask;
}

void
gpioInterruptClear (const unsigned int pin)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	GPEDS[bank] = mask;
}
