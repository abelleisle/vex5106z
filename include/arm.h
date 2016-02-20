#ifndef INCLUDE_ARM_H_
#define INCLUDE_ARM_H_

#include <stdint.h>

typedef void (*funcptr_t)(void);

volatile struct _ExceptionTable {
	funcptr_t unknown1;
	funcptr_t entry;
	funcptr_t NMIException;
	funcptr_t HardFaultException;
	funcptr_t MemManageException;
	funcptr_t BusFaultException;
	funcptr_t UsageFaultException;
	funcptr_t unknown2[4];
	funcptr_t SVCHandler;
	funcptr_t DebugMonitor;
	funcptr_t unknown3;
	funcptr_t PendSVC;
	funcptr_t SysTickHandler;
	funcptr_t InterruptHandler[];
} __attribute__ ((packed)) *ExceptionTable;

volatile struct _SystemControlBlock {
	uint32_t CPUID;
	uint32_t ICSR;
	uint32_t VTOR;
	uint32_t AIRCR;
	uint32_t SCR;
	uint32_t CCR;
	uint32_t SHPR[3];
	uint32_t SHCRS;
	uint8_t  MMSR;
	uint8_t  BFSR;
	uint16_t UFSR;
	uint32_t HFSR;
	uint32_t unused1;
	uint32_t MMAR;
	uint32_t BFAR;
	uint32_t AFSR;
} __attribute__ ((packed)) *SystemControlBlock = (struct _SystemControlBlock *)0xE000ED00;

/**
 * "Allow" divide by zero instructions to be run.
 *
 * By setting this bit in the CCR the Cortex will handle divide by zero
 * instructions by returning a quotient of 0.
 */

void handleDiv0(void){
	SystemControlBlock->CCR |= (1<<4);
}

#endif // INCLUDE_ARM_H_
