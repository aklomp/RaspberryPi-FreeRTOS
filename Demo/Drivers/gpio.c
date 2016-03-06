/**
 *	Quick and very Dirty GPIO API.
 *
 **/

#include "gpio.h"

#define BANK(pin)	((pin) >> 5)
#define MASK(pin)	(1UL << ((pin) & 0x1F))

typedef struct {
	unsigned long	GPFSEL[6];	///< Function selection registers.
	unsigned long	Reserved_1;
	unsigned long	GPSET[2];
	unsigned long	Reserved_2;
	unsigned long	GPCLR[2];
	unsigned long	Reserved_3;
	unsigned long	GPLEV[2];
	unsigned long	Reserved_4;
	unsigned long	GPEDS[2];
	unsigned long	Reserved_5;
	unsigned long	GPREN[2];
	unsigned long	Reserved_6;
	unsigned long	GPFEN[2];
	unsigned long	Reserved_7;
	unsigned long	GPHEN[2];
	unsigned long	Reserved_8;
	unsigned long	GPLEN[2];
	unsigned long	Reserved_9;
	unsigned long	GPAREN[2];
	unsigned long	Reserved_A;
	unsigned long	GPAFEN[2];
	unsigned long	Reserved_B;
	unsigned long	GPPUD[1];
	unsigned long	GPPUDCLK[2];
	//Ignoring the reserved and test bytes
} BCM2835_GPIO_REGS;

volatile BCM2835_GPIO_REGS * const pRegs = (BCM2835_GPIO_REGS *) (0x20200000);

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

void
gpioDetectEnable (const unsigned int pin, const enum GpioDetect type)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	switch (type)
	{
	case GPIO_DETECT_RISING:
		pRegs->GPREN[bank] |= mask;
		break;
	case GPIO_DETECT_FALLING:
		pRegs->GPFEN[bank] |= mask;
		break;
	case GPIO_DETECT_HIGH:
		pRegs->GPHEN[bank] |= mask;
		break;
	case GPIO_DETECT_LOW:
		pRegs->GPLEN[bank] |= mask;
		break;
	case GPIO_DETECT_RISING_ASYNC:
		pRegs->GPAREN[bank] |= mask;
		break;
	case GPIO_DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[bank] |= mask;
		break;
	case GPIO_DETECT_NONE:
		break;
	}
}

void
gpioDetectDisable (const unsigned int pin, const enum GpioDetect type)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	switch (type)
	{
	case GPIO_DETECT_RISING:
		pRegs->GPREN[bank] &= ~mask;
		break;
	case GPIO_DETECT_FALLING:
		pRegs->GPFEN[bank] &= ~mask;
		break;
	case GPIO_DETECT_HIGH:
		pRegs->GPHEN[bank] &= ~mask;
		break;
	case GPIO_DETECT_LOW:
		pRegs->GPLEN[bank] &= ~mask;
		break;
	case GPIO_DETECT_RISING_ASYNC:
		pRegs->GPAREN[bank] &= ~mask;
		break;
	case GPIO_DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[bank] &= ~mask;
		break;
	case GPIO_DETECT_NONE:
		break;
	}
}

void
gpioInterruptClear (const unsigned int pin)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	pRegs->GPEDS[bank] = mask;
}
