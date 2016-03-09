#ifndef _GPIO_H_
#define _GPIO_H_

// GPIO event detect types
enum GpioDetect {
	GPIO_DETECT_RISING,
	GPIO_DETECT_FALLING,
	GPIO_DETECT_HIGH,
	GPIO_DETECT_LOW,
	GPIO_DETECT_RISING_ASYNC,
	GPIO_DETECT_FALLING_ASYNC,
};

// GPIO pull up or down states
enum GpioPull {
	GPIO_PULL_DISABLE,
	GPIO_PULL_UP,
	GPIO_PULL_DOWN,
	GPIO_PULL_RESERVED,
};

// Pin functions
enum GpioFunc {
	GPIO_FUNC_INPUT		= 0,	// Pin is input
	GPIO_FUNC_OUTPUT	= 1,	// Pin is output
	GPIO_FUNC_ALT_0		= 4,	// Alternative function 0
	GPIO_FUNC_ALT_1		= 5,	// Alternative function 1
	GPIO_FUNC_ALT_2		= 6,	// Alternative function 2
	GPIO_FUNC_ALT_3		= 7,	// Alternative function 3
	GPIO_FUNC_ALT_4		= 3,	// Alternative function 4
	GPIO_FUNC_ALT_5		= 2,	// Alternative function 5
};

// Set GPIO pin function:
void gpioFunctionSet (const unsigned int pin, const enum GpioFunc func);

// Get GPIO pin function:
enum GpioFunc gpioFunctionGet (const unsigned int pin);

// Set GPIO output level:
void gpioWrite (const unsigned int pin, const unsigned int val);

// Read GPIO pin level:
unsigned int gpioRead (const unsigned int pin);

// GPIO pull up/down resistor control function (NOT YET IMPLEMENTED):
int gpioPud (const unsigned int pin, const enum GpioPull state);

// Event detection functions:
void gpioDetectEnable     (const unsigned int pin, const enum GpioDetect type);
void gpioDetectDisable    (const unsigned int pin, const enum GpioDetect type);
unsigned int gpioDetected (const unsigned int pin);

#endif
