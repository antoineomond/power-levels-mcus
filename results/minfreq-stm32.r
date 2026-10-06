source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "HSI" & vreg_output == "1.08-1.20V" & clock_freq %/% 1000000 == 1) |
		(clock_source == "HSE" & vreg_output == "1.08-1.20V" & clock_freq %/% 1000000 == 1) |
		(clock_source == "PLL" & vreg_output == "1.26-1.38V" & clock_freq %/% 1000000 == 1))
cp <- get_plot(combined_df)
pdf(paste(folder, "minfreq.pdf", sep=""))
print(cp)
dev.off()
