#include "MK22F12.h"

/* Configure clocks to use external crystal oscillator (X1) rated at 16Mhz.
   System clock is set to 120Mhz, Bus clock to 60Mhz, FlexBus clock to 40Mhz
   and Flash clock to 24Mhz. */
void Configure_Clocks(void) {
	/* Set clock dividers (12.2.15). */

	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0b0000) | // System  = MCGOUTCLK / 1
		      SIM_CLKDIV1_OUTDIV2(0b0001) | // Bus     = MCGOUTCLK / 2
		      SIM_CLKDIV1_OUTDIV3(0b0010) | // FlexBus = MCGOUTCLK / 3
		      SIM_CLKDIV1_OUTDIV4(0b0100);  // Flash   = MCGOUTCLK / 5

	/* Switching procedure from FEI to PEE as described in 25.5.3.1. */

	/* 1. Switch from FEI to FBE mode. */

	/* Select very high frequency range and high-gain operation. */
	MCG_C2 |= MCG_C2_RANGE0(2) | MCG_C2_HGO0_MASK;
	MCG_C2 |= MCG_C2_EREFS0_MASK; // Request oscillator

	/* Select external oscillator as system clock source. */
	MCG_C1 = (MCG_C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(0b10);
	/* Divide by 512 (16Mhz / 512 = 31.25KHz). */
	MCG_C1 = (MCG_C1 & ~MCG_C1_FRDIV_MASK) | MCG_C1_FRDIV(0b100);
	MCG_C1 = MCG_C1 & ~MCG_C1_IREFS_MASK; // Enable external oscillator

	while (!(MCG_S & MCG_S_OSCINIT0_MASK));
	while (MCG_S & MCG_S_IREFST_MASK);
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0b10);

	/* 2. Setup PRDIV0 to generate correct PLL reference frequency. */
	MCG_C5 |= MCG_C5_PRDIV0(0b11); // Divide by 4 (16Mhz / 4 = 4MHz)

	/* 3. Switch from FBE directly to PBE. */

	MCG_C6 |= MCG_C6_PLLS_MASK; // Select PLL
	MCG_C6 |= MCG_C6_VDIV0(0b00110); // Select M = 30 (4MHz * 30 = 120Mhz)

	while (!(MCG_S & MCG_S_PLLST_MASK));
	while (!(MCG_S & MCG_S_LOCK0_MASK));

	/* 4. Switch from PBE to PEE. */

	MCG_C1 = (MCG_C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(0b00);
	while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0b11);

	/* MCGOUTCLK frequency is now (External Crystal / DIV) * M = 120Mhz */

	/* Enable OSCERCLK (even in Stop mode) */
	//OSC_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK;
}
