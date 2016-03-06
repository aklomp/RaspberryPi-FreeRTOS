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

// Pin data direction
enum GpioDir {
	GPIO_DIR_IN,
	GPIO_DIR_OUT,
};

// GPIO pin setup:
void gpioFunctionSet (const unsigned int pin, const unsigned int func);

// A simple wrapper around SetGpioFunction:
void gpioDirectionSet (const unsigned int pin, const enum GpioDir dir);

// Set GPIO output level:
void gpioWrite (const unsigned int pin, const unsigned int val);

// Read GPIO pin level:
unsigned int gpioRead (const unsigned int pin);

// GPIO pull up/down resistor control function (NOT YET IMPLEMENTED):
int gpioPud (const unsigned int pin, const enum GpioPull state);

// Interrupt related functions:
void gpioDetectEnable   (const unsigned int pin, const enum GpioDetect type);
void gpioDetectDisable  (const unsigned int pin, const enum GpioDetect type);
void gpioInterruptClear (const unsigned int pin);

#endif
