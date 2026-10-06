source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "0.90V" & clock_freq %/% 1000000 == 150) |
		(clock_source == "ROSC" & vreg_output == "0.85V" & clock_freq %/% 1000000 == 4) |
		(clock_source == "XOSC" & vreg_output == "0.85V" & clock_freq %/% 1000000 == 12))
cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "minvoltage.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()
