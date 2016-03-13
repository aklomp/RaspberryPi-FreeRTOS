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

// Pull up/down/none:
#define GPPUD		GPIO_REG (0x0094)
#define GPPUDCLK	GPIO_REG (0x0098)

void
gpioFunctionSet (const unsigned int pin, const enum GpioFunc func)
{
	unsigned long bank = pin / 10;
	unsigned long item = pin % 10;
	unsigned long mask = 7UL << (item * 3);

	// Get current register value:
	unsigned long reg = GPFSEL[bank];

	// Mask out the bits for this pin:
	reg &= ~mask;

	// Insert new bits for this pin:
	reg |= (unsigned long)func << (item * 3);

	// Store back:
	GPFSEL[bank] = reg;
}

enum GpioFunc
gpioFunctionGet (const unsigned int pin)
{
	unsigned long bank = pin / 10;
	unsigned long item = pin % 10;

	return (GPFSEL[bank] >> (item * 3)) & 7;
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

unsigned int
gpioDetected (const unsigned int pin)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	// No event if bit not set:
	if (!(GPEDS[bank] & mask))
		return 0;

	// Clear event by setting bit to 1:
	GPEDS[bank] |= mask;

	// Confirm event:
	return 1;
}

static inline void
wait (unsigned long cycles)
{
	// Each loop iteration takes three instructions:
	cycles /= 3;

	// Busy-wait by decrementing the cycle counter until it reaches zero:
	asm volatile (
		"1: sub %0, %0, #1  \n\t"
		"   cmp %0, #0      \n\t"
		"   bne 1b          \n\t"
		:
		: "r" (cycles)
		: "cc"
	);
}

void
gpioPull (const unsigned int pin, const enum GpioPull type)
{
	unsigned long bank = BANK(pin);
	unsigned long mask = MASK(pin);

	// Setup direction register:
	*GPPUD = type;

	// Wait 150 cycles to set up the control signal:
	wait(150);

	// Activate clock signal for this pin:
	GPPUDCLK[bank] |= mask;

	// Wait 150 cycles to hold the control signal:
	wait(150);

	// Remove control signal:
	*GPPUD = 0;

	// Remove clock signal:
	GPPUDCLK[bank] = 0;
}
