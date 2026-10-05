source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 15) |
		(clock_source == "ROSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 2))
cp <- get_plot(combined_df)
pdf(paste(folder, "minfreq.pdf", sep=""))
print(cp)
dev.off()
