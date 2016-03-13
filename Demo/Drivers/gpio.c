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


void SetGpioFunction(unsigned int pinNum, unsigned int funcNum) {

	int offset = pinNum / 10;

	unsigned long val = pRegs->GPFSEL[offset];	// Read in the original register value.

	int item = pinNum % 10;
	val &= ~(0x7 << (item * 3));
	val |= ((funcNum & 0x7) << (item * 3));
	pRegs->GPFSEL[offset] = val;
}

void SetGpioDirection(unsigned int pinNum, enum GPIO_DIR dir) {
	SetGpioFunction(pinNum,dir);
}

void SetGpio(unsigned int pinNum, unsigned int pinVal)
{
	unsigned long bank = BANK(pinNum);
	unsigned long mask = MASK(pinNum);

	if(pinVal) {
		pRegs->GPSET[bank] |= mask;
	} else {
		pRegs->GPCLR[bank] |= mask;
	}
}

int ReadGpio(unsigned int pinNum)
{
	unsigned long bank = BANK(pinNum);
	unsigned long mask = MASK(pinNum);

	return ((pRegs->GPLEV[bank]) >> mask) & 1;
}

void EnableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type)
{
	unsigned long bank = BANK(pinNum);
	unsigned long mask = MASK(pinNum);

	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[bank] |= mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[bank] |= mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[bank] |= mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[bank] |= mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[bank] |= mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[bank] |= mask;
		break;
	case DETECT_NONE:
		break;
	}
}

void DisableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type)
{
	unsigned long bank = BANK(pinNum);
	unsigned long mask = MASK(pinNum);

	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[bank] &= ~mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[bank] &= ~mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[bank] &= ~mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[bank] &= ~mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[bank] &= ~mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[bank] &= ~mask;
		break;
	case DETECT_NONE:
		break;
	}
}

void ClearGpioInterrupt(unsigned int pinNum)
{
	unsigned long bank = BANK(pinNum);
	unsigned long mask = MASK(pinNum);

	pRegs->GPEDS[bank] = mask;
}
