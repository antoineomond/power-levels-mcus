source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "PLL64" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 1) |
		(clock_source == "PLL96" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 1))
cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "minfreq.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()
