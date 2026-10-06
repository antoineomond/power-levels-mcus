source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "1.08-1.20V" & clock_freq %/% 1000000 == 64))
cp <- get_plot(combined_df)
pdf(paste(folder, "minvreg.pdf", sep=""))
print(cp)
dev.off()
