// MIDI KEYBOARD -- Matteo Biffoni
// Sleep utility
#ifndef __SLEEP_UTIL_INTERFACE_H
#define __SLEEP_UTIL_INTERFACT_H
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

#define sleep(mode) do {\
    uint8_t __sreg_bak = SREG;\
    set_sleep_mode(mode);\
    sleep_enable();\
    sei();\
    sleep_cpu();\
    sleep_disable();\
    SREG = __sreg_bak;\
} while(0);

#define sleep_on(mode,expr) do {\
  uint8_t __sreg_bak = SREG;\
  set_sleep_mode(mode);\
  cli();\
  if (expr) {\
    sleep_enable();\
    sei();\
    sleep_cpu();\
    sleep_disable();\
  }\
  SREG = __sreg_bak;\
} while (0)

#define sleep_while(mode,expr) do {\
  uint8_t __sreg_bak = SREG;\
  set_sleep_mode(mode);\
  while (1) {\
    cli();\
    if (expr) {\
      sleep_enable();\
      sei();\
      sleep_cpu();\
      sleep_disable();\
    }\
    else break;\
  }\
  SREG = __sreg_bak;\
} while (0)

#endif  // __SLEEP_UTIL_INTERFACE_H