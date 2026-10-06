source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "HSI" & vreg_output == "1.08-1.20V" & clock_freq %/% 1000000 == 16) |
		(clock_source == "HSE" & vreg_output == "1.08-1.20V" & clock_freq %/% 1000000 == 25) |
		(clock_source == "PLL" & vreg_output == "1.26-1.38V" & clock_freq %/% 1000000 == 64))
cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "baseline.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()
