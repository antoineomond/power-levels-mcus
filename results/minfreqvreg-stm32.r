source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL" & vreg_output == "Range3" & clock_freq %/% 1000000 == 1))
cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "minfreqvoltage.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()
