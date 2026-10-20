#include <stdint.h>
#include "soc/rtc.h"

typedef enum CLK_SRC {
  XTAL=1, PLL_64M=2, PLL_96M=3, RC=4
 } CLK_SRC;

static inline bool set_cpu_clock(CLK_SRC source, uint32_t divider) {
  rtc_cpu_freq_config_t setup;

  if (source == XTAL){
	setup.source_freq_mhz = (uint32_t)rtc_clk_xtal_freq_get();
	setup.source = SOC_CPU_CLK_SRC_XTAL;
  }
  else if (source == PLL_64M){
	setup.source_freq_mhz = 64;
	setup.source = SOC_CPU_CLK_SRC_FLASH_PLL;
  }
  else if (source == PLL_96M){
	setup.source_freq_mhz = 96;
	setup.source = SOC_CPU_CLK_SRC_PLL;
  }
  else if (source == RC){
	setup.source_freq_mhz = 8;
	setup.source = SOC_CPU_CLK_SRC_RC_FAST;
  } else {
	return 0;
  }

  if (divider < 1)
	return 0;

  setup.div = divider;
  setup.freq_mhz = (setup.source_freq_mhz + divider/2) / divider;

  rtc_clk_cpu_freq_set_config(&setup);

  return 1;
}

