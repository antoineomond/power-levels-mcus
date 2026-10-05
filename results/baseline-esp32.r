source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL64" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 64) |
		(clock_source == "PLL96" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 96) |
		(clock_source == "RC_FAST" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 8))
cp <- get_plot(combined_df)
pdf(paste(folder, "baseline.pdf", sep=""))
print(cp)
dev.off()
