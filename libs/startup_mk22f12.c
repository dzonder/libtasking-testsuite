#include "MK22F12.h"

#define __NVIC_PRIO_BITS	3

/** Common startup procedure **/

/* Following variables are defined in the linker script. */
extern unsigned long _sidata;   /* first 4 bytes of initialized data in FLASH */
extern unsigned long _sdata;    /* first 4 bytes of initialized data in RAM */
extern unsigned long _edata;    /* 4 bytes right after initialized data in RAM */
extern unsigned long _sbss;     /* first 4 bytes of uninitialized data in RAM */
extern unsigned long _ebss;     /* 4 bytes right after uninitialized data in RAM */
extern unsigned long _estack;   /* initial top of the stack */

extern void Configure_Clocks(void);

int __errno;

int main(void);

static void Default_HardFault_Handler(void) {
	/* Fix link-register and halt the MCU. */
	__asm volatile("tst lr, #4\n\t"		// MSP or PSP?
			"ite eq\n\t"
			"mrseq r0, msp\n\t"
			"mrsne r0, psp\n\t"
			"ldr r2, [r0, #24]\n\t"	// old LR = *(r0 + 0x18)
			"mov lr, r2\n\t"
			"ldr r2, [r0, #8]\n\t"
			"ldr r0, [r0]\n\t"
			"bkpt\n\t");
}

static void Default_Handler(void) {
	__asm volatile("bkpt");
}

static void Disable_Watchdog(void) {
	/* There are 2 unlock words which shall be provided in sequence
	   before accessing the control register. */
	WDOG_UNLOCK = 0xC520;
	WDOG_UNLOCK = 0xD928;
	WDOG_STCTRLH = 0xD2;
}

static void Enable_Fpu(void) {
	SCB_CPACR |= SCB_CPACR_CP10(0b11) | SCB_CPACR_CP11(0b11);
	__asm volatile("dsb; isb;" ::);
}

/* Procedure called after MCU reset */
static void Reset_Handler(void) {
	unsigned long *src, *dst;
	Disable_Watchdog();
	Configure_Clocks();
	/* Copy initialized data from FLASH to RAM. */
	for (dst = &_sdata, src = &_sidata; dst < &_edata; ++dst, ++src)
		*dst = *src;
	/* Set uninitialized data to zeros. */
	for (dst = &_sbss; dst < &_ebss; ++dst)
		*dst = 0;
	Enable_Fpu();
	/* Set device specific interrupt priorities to 8 */
	for (uint32_t irq_n = INT_DMA0; irq_n <= INT_SDHC; ++irq_n)
		NVIC_BASE_PTR->IP[irq_n] = 2 << (8 - __NVIC_PRIO_BITS);
	/* Call main program function - usually never returns. */
	main();
	/* In case it returns - endless loop. */
hang:
	goto hang;
}

/** Interrupt handler declarations **/

#define WEAK_FUN(func)		__attribute__ ((weak, alias(func)))
#define WEAK			WEAK_FUN("Default_Handler");

/* ARM core interrupts */
void NMI_Handler() WEAK;
void HardFault_Handler() WEAK_FUN("Default_HardFault_Handler");
void Mem_Manage_Fault_Handler() WEAK;
void Bus_Fault_Handler() WEAK;
void Usage_Fault_Handler() WEAK;
void SVC_Handler() WEAK;
void DebugMonitor_Handler() WEAK;
void PendSV_Handler() WEAK;
void SysTick_Handler() WEAK;

/* Peripheral interrupts */
void DMA0_IRQHandler() WEAK;
void DMA1_IRQHandler() WEAK;
void DMA2_IRQHandler() WEAK;
void DMA3_IRQHandler() WEAK;
void DMA4_IRQHandler() WEAK;
void DMA5_IRQHandler() WEAK;
void DMA6_IRQHandler() WEAK;
void DMA7_IRQHandler() WEAK;
void DMA8_IRQHandler() WEAK;
void DMA9_IRQHandler() WEAK;
void DMA10_IRQHandler() WEAK;
void DMA11_IRQHandler() WEAK;
void DMA12_IRQHandler() WEAK;
void DMA13_IRQHandler() WEAK;
void DMA14_IRQHandler() WEAK;
void DMA15_IRQHandler() WEAK;
void DMA_Error_IRQHandler() WEAK;
void MCM_IRQHandler() WEAK;
void FTFL_IRQHandler() WEAK;
void Read_Collision_IRQHandler() WEAK;
void LVD_LVW_IRQHandler() WEAK;
void LLW_IRQHandler() WEAK;
void Watchdog_IRQHandler() WEAK;
void RNG_IRQHandler() WEAK;
void I2C0_IRQHandler() WEAK;
void I2C1_IRQHandler() WEAK;
void SPI0_IRQHandler() WEAK;
void SPI1_IRQHandler() WEAK;
void I2S0_Tx_IRQHandler() WEAK;
void I2S0_Rx_IRQHandler() WEAK;
void UART0_LON_IRQHandler() WEAK;
void UART0_Status_IRQHandler() WEAK;
void UART0_ERR_IRQHandler() WEAK;
void UART1_Status_IRQHandler() WEAK;
void UART1_ERR_IRQHandler() WEAK;
void UART2_Status_IRQHandler() WEAK;
void UART2_ERR_IRQHandler() WEAK;
void UART3_Status_IRQHandler() WEAK;
void UART3_ERR_IRQHandler() WEAK;
void ADC0_IRQHandler() WEAK;
void CMP0_IRQHandler() WEAK;
void CMP1_IRQHandler() WEAK;
void FTM0_IRQHandler() WEAK;
void FTM1_IRQHandler() WEAK;
void FTM2_IRQHandler() WEAK;
void CMT_IRQHandler() WEAK;
void RTC_Alarm_IRQHandler() WEAK;
void RTC_Seconds_IRQHandler() WEAK;
void PIT0_IRQHandler() WEAK;
void PIT1_IRQHandler() WEAK;
void PIT2_IRQHandler() WEAK;
void PIT3_IRQHandler() WEAK;
void PDB_IRQHandler() WEAK;
void USB_OTG_IRQHandler() WEAK;
void USB_Charger_Detect_IRQHandler() WEAK;
void Tamper_Drylce_IRQHandler() WEAK;
void DAC0_IRQHandler() WEAK;
void MCG_IRQHandler() WEAK;
void LPTimer_IRQHandler() WEAK;
void PORTA_IRQHandler() WEAK;
void PORTB_IRQHandler() WEAK;
void PORTC_IRQHandler() WEAK;
void PORTD_IRQHandler() WEAK;
void PORTE_IRQHandler() WEAK;
void SWI_IRQHandler() WEAK;
void SPI2_IRQHandler() WEAK;
void UART4_Status_IRQHandler() WEAK;
void UART4_ERR_IRQHandler() WEAK;
void CMP2_IRQHandler() WEAK;
void FTM3_IRQHandler() WEAK;
void DAC1_IRQHandler() WEAK;
void ADC1_IRQHandler() WEAK;
void I2C2_IRQHandler() WEAK;
void CAN0_Message_buffer_IRQHandler() WEAK;
void CAN0_Bus_Off_IRQHandler() WEAK;
void CAN0_Err_IRQHandler() WEAK;
void CAN0_Tx_WarningIRQHandler() WEAK;
void CAN0_Rx_Warning_IRQHandler() WEAK;
void CAN0_Wake_Up_IRQHandler() WEAK;
void SDHC_IRQHandler() WEAK;

/** Interrupt table **/

__attribute__ ((used))
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	/* Initial values of SP and PC */
	(void*)&_estack,
	Reset_Handler,

	/* ARM core interrupts */
	NMI_Handler,
	HardFault_Handler,
	Mem_Manage_Fault_Handler,
	Bus_Fault_Handler,
	Usage_Fault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMonitor_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,

	/* Interrupts */
	DMA0_IRQHandler, /* DMA Channel 0 Transfer Complete */
	DMA1_IRQHandler, /* DMA Channel 1 Transfer Complete */
	DMA2_IRQHandler, /* DMA Channel 2 Transfer Complete */
	DMA3_IRQHandler, /* DMA Channel 3 Transfer Complete */
	DMA4_IRQHandler, /* DMA Channel 4 Transfer Complete */
	DMA5_IRQHandler, /* DMA Channel 5 Transfer Complete */
	DMA6_IRQHandler, /* DMA Channel 6 Transfer Complete */
	DMA7_IRQHandler, /* DMA Channel 7 Transfer Complete */
	DMA8_IRQHandler, /* DMA Channel 8 Transfer Complete */
	DMA9_IRQHandler, /* DMA Channel 9 Transfer Complete */
	DMA10_IRQHandler, /* DMA Channel 10 Transfer Complete */
	DMA11_IRQHandler, /* DMA Channel 11 Transfer Complete */
	DMA12_IRQHandler, /* DMA Channel 12 Transfer Complete */
	DMA13_IRQHandler, /* DMA Channel 13 Transfer Complete */
	DMA14_IRQHandler, /* DMA Channel 14 Transfer Complete */
	DMA15_IRQHandler, /* DMA Channel 15 Transfer Complete */
	DMA_Error_IRQHandler, /* DMA Error */
	MCM_IRQHandler, /* Normal Interrupt */
	FTFL_IRQHandler, /* FTFL Interrupt */
	Read_Collision_IRQHandler,
	LVD_LVW_IRQHandler, /* Low Voltage Detect, Low Voltage Warning */
	LLW_IRQHandler, /* Low Leakage Wake-up */
	Watchdog_IRQHandler,
	RNG_IRQHandler,
	I2C0_IRQHandler, /* I2C0 interrupt */
	I2C1_IRQHandler, /* I2C1 interrupt */
	SPI0_IRQHandler, /* SPI0 Interrupt */
	SPI1_IRQHandler, /* SPI1 Interrupt */
	I2S0_Tx_IRQHandler, /* Transmit I2S0 interrupt */
	I2S0_Rx_IRQHandler, /* Receive I2S0 interrupt */
	UART0_LON_IRQHandler, /* UART0 LON interrupt */
	UART0_Status_IRQHandler, /* UART0 Status interrupt */
	UART0_ERR_IRQHandler, /* UART0 Error interrupt */
	UART1_Status_IRQHandler, /* UART1 Status interrupt */
	UART1_ERR_IRQHandler, /* UART1 Error interrupt */
	UART2_Status_IRQHandler, /* UART2 Status interrupt */
	UART2_ERR_IRQHandler, /* UART2 Error interrupt */
	UART3_Status_IRQHandler, /* UART3 Status interrupt */
	UART3_ERR_IRQHandler, /* UART3 Error interrupt */
	ADC0_IRQHandler, /* ADC0 interrupt */
	CMP0_IRQHandler, /* CMP0 interrupt */
	CMP1_IRQHandler, /* CMP1 interrupt */
	FTM0_IRQHandler, /* FTM0 fault, overflow and channels interrupt */
	FTM1_IRQHandler, /* FTM1 fault, overflow and channels interrupt */
	FTM2_IRQHandler, /* FTM2 fault, overflow and channels interrupt */
	CMT_IRQHandler,
	RTC_Alarm_IRQHandler, /* RTC Alarm interrupt */
	RTC_Seconds_IRQHandler, /* RTC Seconds interrupt */
	PIT0_IRQHandler, /* PIT timer all channels interrupt */
	PIT1_IRQHandler, /* PIT timer all channels interrupt */
	PIT2_IRQHandler, /* PIT timer all channels interrupt */
	PIT3_IRQHandler, /* PIT timer all channels interrupt */
	PDB_IRQHandler,
	USB_OTG_IRQHandler,
	USB_Charger_Detect_IRQHandler,
	Tamper_Drylce_IRQHandler,
	DAC0_IRQHandler, /* DAC0 interrupt */
	MCG_IRQHandler, /* MCG Interrupt */
	LPTimer_IRQHandler, /* LPTimer interrupt */
	PORTA_IRQHandler, /* Port A interrupt */
	PORTB_IRQHandler, /* Port B interrupt */
	PORTC_IRQHandler, /* Port C interrupt */
	PORTD_IRQHandler, /* Port D interrupt */
	PORTE_IRQHandler, /* Port E interrupt */
	SWI_IRQHandler,
	SPI2_IRQHandler,
	UART4_Status_IRQHandler,
	UART4_ERR_IRQHandler,
	0,
	0,
	CMP2_IRQHandler,
	FTM3_IRQHandler,
	DAC1_IRQHandler,
	ADC1_IRQHandler,
	I2C2_IRQHandler,
	CAN0_Message_buffer_IRQHandler,
	CAN0_Bus_Off_IRQHandler,
	CAN0_Err_IRQHandler,
	CAN0_Tx_WarningIRQHandler,
	CAN0_Rx_Warning_IRQHandler,
	CAN0_Wake_Up_IRQHandler,
	SDHC_IRQHandler,
};

typedef struct {
	/* Bytes 0x000003FC and 0x000003FD */
	unsigned reserved_3FC_3FD      : 16;
	/* Byte 0x000003FE */
	unsigned scftrim               :  1;
	unsigned fctrim                :  4;
	unsigned reserved_3FE_5_7      :  3;
	/* Byte 0x000003FF */
	unsigned sctrim                :  8;
	/* Bytes 0x00000400 to 0x00000407 */
	unsigned backdoor_key_l;
	unsigned backdoor_key_h;
	/* Bytes 0x00000408 to 0x0000040B */
	unsigned ftfe_fprot0           :  8;
	unsigned ftfe_fprot1           :  8;
	unsigned ftfe_fprot2           :  8;
	unsigned ftfe_fprot3           :  8;
	/* Byte 0x0000040C */
	unsigned ftfe_fsec_sec         :  2;
	unsigned ftfe_fsec_fslacc      :  2;
	unsigned ftfe_fsec_meen        :  2;
	unsigned ftfe_fsec_keyen       :  2;
	/* Byte 0x0000040D */
	unsigned fopt_lpboot0          :  1;
	unsigned fopt_reserved_40D_1   :  1;
	unsigned fopt_nmi_dis          :  1;
	unsigned fopt_reset_pin_cfg    :  1;
	unsigned fopt_lpboot1          :  1;
	unsigned fopt_fast_init        :  1;
	unsigned fopt_reserved_40D_6_7 :  2;
	/* Bytes 0x0000040E and 0x0000040F */
	unsigned reserved_40E          :  8;
	unsigned reserved_40F          :  8;
} run_options_t;

/* Default values are all ones. */
__attribute__ ((section(".run_options")))
run_options_t run_options = {
	0xFFFF, 0x1, 0xF, 0x7, 0xFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0x3, 0x3, 0x3, 0x3, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x3, 0xFF, 0xFF
};
