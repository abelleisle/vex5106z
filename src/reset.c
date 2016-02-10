#include <main.h>

/**
 * Cause the Cortex to reset, which results in the Cortex restarting the
 * operator control code.
 *
 * This reset is accomplished through setting two bits, SYSRESETREQ and
 * VECTRESET, in the Application Interrupt and Reset Control Register (AIRCR),
 * which is at a known location in memory. SYSRESETREQ will actually request
 * the system reset, while VECTRESET is 'reserved for debugging'. This second
 * bit may not need to be set; it's only set here because others have found it
 * necessary.
 */

#define AIRCR_ADDR		0xE000ED0C
#define VECTKEY			0x05FA
#define SYSRESETREQ		(1<<2)
#define VECTRESET		(1<<0)

void softwareReset(void){

	/*
	 * Read the current value of the AIRCR, since some flags currently set in
	 * the register need to remain the same in order for the reset to work.
	 */

	uint32_t AIRCR = *((uint32_t *)AIRCR_ADDR);

	/*
	 * Here we save the lower 16 bits of the register, write a special key to
	 * the higher 16 bits that'll allow the write to occur, and then set the
	 * reset flags.
	 */

	AIRCR = (AIRCR & 0xFFFF) | (VECTKEY << 16) | SYSRESETREQ | VECTRESET;

	// Update the AIRCR.

	*((uint32_t *)0xE000ED0C) = AIRCR;

	/*
	 * This instruction causes the program to wait until the previous memory
	 * write is complete, ensuring it is taken into effect and the reset
	 * request is made.
	 */

	asm("DSB");

	// Wait until the system reset completes.

	while(1);
}
