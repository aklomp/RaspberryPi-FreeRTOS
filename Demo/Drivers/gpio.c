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

// Pin event detection:
#define GPREN		GPIO_REG (0x004C)
#define GPFEN		GPIO_REG (0x0058)
#define GPHEN		GPIO_REG (0x0064)
#define GPLEN		GPIO_REG (0x0070)
#define GPAREN		GPIO_REG (0x007C)
#define GPAFEN		GPIO_REG (0x0088)

typedef struct {
	unsigned long GPFSEL[6];	///< Function selection registers.
	unsigned long Reserved_1;
	unsigned long GPSET[2];
	unsigned long Reserved_2;
	unsigned long GPCLR[2];
	unsigned long Reserved_3;
	unsigned long GPLEV[2];
	unsigned long Reserved_4;
	unsigned long GPEDS[2];
	unsigned long Reserved_5;
} __attribute__((packed))
BCM2835_GPIO_REGS;

volatile BCM2835_GPIO_REGS * const pRegs = (BCM2835_GPIO_REGS *) (GPIO_REG_BASE);

void
gpioFunctionSet (const unsigned int pin, const unsigned int func)
{
	int offset = pin / 10;

	unsigned long val = pRegs->GPFSEL[offset];	// Read in the original register value.

	int item = pin % 10;
	val &= ~(0x7 << (item * 3));
	val |= ((func & 0x7) << (item * 3));
	pRegs->GPFSEL[offset] = val;
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

	val ? (pRegs->GPSET[bank] |= mask)
	    : (pRegs->GPCLR[bank] |= mask);
}

unsigned int
gpioRead (const unsigned int pin)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	return ((pRegs->GPLEV[bank]) >> mask) & 1;
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

	pRegs->GPEDS[bank] = mask;
}
