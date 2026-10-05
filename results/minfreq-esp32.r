source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL64" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 1) |
		(clock_source == "PLL96" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 1))
cp <- get_plot(combined_df)
pdf(paste(folder, "minfreq.pdf", sep=""))
print(cp)
dev.off()
