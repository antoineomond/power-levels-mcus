source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 150) |
		(clock_source == "ROSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 10) |
		(clock_source == "XOSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 12))
cp <- get_plot(combined_df)
pdf(paste(folder, "baseline.pdf", sep=""))
print(cp)
dev.off()
