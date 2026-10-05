source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "0.90V" & clock_freq %/% 1000000 == 15) |
		(clock_source == "ROSC" & vreg_output == "0.85V" & clock_freq %/% 1000000 == 1))
cp <- get_plot(combined_df)
pdf(paste(folder, "minfreqvoltage.pdf", sep=""))
print(cp)
dev.off()
