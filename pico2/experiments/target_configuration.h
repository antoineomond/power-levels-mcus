#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/powman.h"
#include "hardware/pll.h"
#include "hardware/xosc.h"
#include "hardware/ticks.h"
#include "hardware/watchdog.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"

enum CLOCK_SOURCE {
	PLL_SYS, XOSC, ROSC, LPOSC
};

typedef struct config {
	// Clock source
	uint clock_source;
	
	// Clock frequency
	// PLL
  uint pll_vco_freq;
  uint pll_div1;
  uint pll_div2;
	
	// ROSC
  uint rosc_div;
  uint rosc_range;
  uint rosc_drive_freqa;
  uint rosc_drive_freqb;
	
	// LPOSC
	uint lposc_trim;
	
	// VREG output
	uint vreg_output;
	
	// Set clock source as reference clock
	bool set_as_ref;
} config;
float TIME_RATE = 1;

// From pico-sdk
static inline void start_all_ticks(void) {
    uint32_t cycles = clock_get_hz(clk_ref) / MHZ;
		if(cycles <= 0) {
			cycles = 1;
		}
    for (int i = 0; i < (int)TICK_COUNT; ++i) {
        tick_start((tick_gen_num_t)i, cycles);
    }
}

// From pico-sdk
static inline void restart_all_ticks(void) {
	for (int i = 0; i < (int)TICK_COUNT; ++i) {
			tick_stop((tick_gen_num_t)i);
			while(tick_is_running((tick_gen_num_t)i)) tight_loop_contents();
	}
	start_all_ticks();
}

static inline void switch_to_default_configuration() {
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, XOSC_HZ);
	restart_all_ticks();
	hw_clear_bits(&powman_hw->vreg_ctrl, POWMAN_PASSWORD_BITS | POWMAN_VREG_CTRL_DISABLE_VOLTAGE_LIMIT_BITS);
	powman_clear_bits(&powman_hw->bod, 0x000001f1);
	powman_set_bits(&powman_hw->bod, POWMAN_BOD_VSEL_RESET);
	vreg_set_voltage(VREG_VOLTAGE_DEFAULT);
	sleep_us((int)(1*1000000*TIME_RATE));
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	pll_init(pll_sys, PLL_SYS_REFDIV, PLL_SYS_VCO_FREQ_HZ, PLL_SYS_POSTDIV1, PLL_SYS_POSTDIV2);
	pll_init(pll_usb, PLL_USB_REFDIV, PLL_USB_VCO_FREQ_HZ, PLL_USB_POSTDIV1, PLL_USB_POSTDIV2);
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX, CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, SYS_CLK_HZ);
	clock_configure_undivided(clk_peri,
									0,
									CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	clock_configure_undivided(clk_usb,
									0, // No GLMUX
									CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_adc,
									0, // No GLMUX
									CLOCKS_CLK_ADC_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
									USB_CLK_HZ);
	clock_configure_undivided(clk_hstx,
									0,
									CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLK_SYS,
									SYS_CLK_HZ);
	
	stdio_init_all();
	sleep_ms(1000);
}

static inline uint set_clock_source_xosc() {
	xosc_init();
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC, 0, XOSC_HZ);
	restart_all_ticks();
	clock_configure_undivided(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF, 0, XOSC_HZ);
	
	uint clk_src_freq = XOSC_HZ; // Known fix frequency
	
	// Disable unused clock sources
	pll_deinit(pll_sys);
	pll_deinit(pll_usb);
	rosc_disable();
	
	TIME_RATE = 1;
	
	return clk_src_freq;
}

// Clock source leverages
static inline uint set_clock_source_lposc(uint trim) {
	// Specify lposc frequency
	powman_clear_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS);
	powman_set_bits(&powman_hw->lposc, POWMAN_LPOSC_TRIM_BITS & (trim << POWMAN_LPOSC_TRIM_LSB));
	sleep_ms(100);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_LPOSC_CLKSRC) * KHZ;
	
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_LPOSC_CLKSRC, 0, clk_src_freq);
	restart_all_ticks();
	TIME_RATE = ((float)clk_src_freq)/((float)1*MHZ); // LPOSC isn't fast enough to generate the 1us tick (hardwired value). The TIME_RATE divides any active wait to account for this slowness
	clock_set_reported_hz(clk_sys, clk_src_freq);
	
	xosc_disable();
	
	return clk_src_freq;
}
static inline uint set_clock_source_rosc(uint div, uint range, uint freqa, uint freqb) {
	rosc_restart();
	
	// Specify rosc frequency
	rosc_set_div(div);
	rosc_set_range(range);
	rosc_write(&rosc_hw->freqa, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqa);
	rosc_write(&rosc_hw->freqb, (ROSC_FREQA_PASSWD_VALUE_PASS << ROSC_FREQA_PASSWD_LSB) | freqb);
	
	uint clk_src_freq = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC) * KHZ;
	
	clock_configure_undivided(clk_ref, CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH, 0, clk_src_freq);
	restart_all_ticks();
	uint divider = ((float)(clk_src_freq/MHZ));
	if(divider == 0) {
		divider = 1;
	}
	TIME_RATE = ((float)clk_src_freq/(float)MHZ)/(float)divider; // clk_ref takes clock_freq/MHz as reference to compute time, trimming all remaining KHz. This leads to incorrect time tracking  
	xosc_disable();
	return clk_src_freq;
}


static inline uint set_clock_source_pll(uint vco_freq, uint div1, uint div2) {
	pll_init(pll_sys, PLL_SYS_REFDIV, vco_freq, div1, div2);
	uint32_t freq = vco_freq / (div1 * div2);
	clock_configure_undivided(clk_sys,
									CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
									CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
									freq);
	return freq;
}

static inline uint switch_configuration_from_parameter(const struct config* config) {
	// Set the voltage, clock source and frequency (measure the frequency for rosc and lposc)
	// pll must be deactivated to reach vreg outputs below 0.9V
	set_clock_source_xosc();
	sleep_ms(100);
	vreg_set_voltage(config->vreg_output);
	sleep_ms(100);
	
	uint clk_src_freq;
	if(config->clock_source == PLL_SYS) {
		clk_src_freq = set_clock_source_pll(config->pll_vco_freq, config->pll_div1, config->pll_div2);
	}
	if(config->clock_source == XOSC) {
		clk_src_freq = set_clock_source_xosc();
	}
	if(config->clock_source == ROSC) {
		clk_src_freq = set_clock_source_rosc(config->rosc_div, config->rosc_range, config->rosc_drive_freqa, config->rosc_drive_freqb);
	}
	if(config->clock_source == LPOSC) {
		clk_src_freq = set_clock_source_lposc(config->lposc_trim);
	}
	return clk_src_freq;
}
