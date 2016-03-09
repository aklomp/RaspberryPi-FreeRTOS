#ifndef _GPIO_H_
#define _GPIO_H_

/* GPIO event detect types */
enum DETECT_TYPE {
	DETECT_NONE,
	DETECT_RISING,
	DETECT_FALLING,
	DETECT_HIGH,
	DETECT_LOW,
	DETECT_RISING_ASYNC,
	DETECT_FALLING_ASYNC
};

/* GPIO pull up or down states */
enum PULL_STATE {
	PULL_DISABLE,
	PULL_UP,
	PULL_DOWN,
	PULL_RESERVED
};

/* Pin data direction */
enum GPIO_DIR {
	GPIO_IN,
	GPIO_OUT
};

// GPIO pin setup:
void gpioFunctionSet (const unsigned int pin, const unsigned int func);

// A simple wrapper around SetGpioFunction:
void gpioDirectionSet (const unsigned int pin, const enum GPIO_DIR dir);

// Set GPIO output level:
void gpioWrite (const unsigned int pin, const unsigned int val);

// Read GPIO pin level:
unsigned int gpioRead (const unsigned int pin);

// GPIO pull up/down resistor control function (NOT YET IMPLEMENTED):
int gpioPud (const unsigned int pin, const enum PULL_STATE state);

// Interrupt related functions:
void gpioDetectEnable   (const unsigned int pin, const enum DETECT_TYPE type);
void gpioDetectDisable  (const unsigned int pin, const enum DETECT_TYPE type);
void gpioInterruptClear (const unsigned int pin);

#endif
