source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "LPOSC" & vreg_output == "1.10V") |
		(clock_source == "LPOSC" & vreg_output == "0.80V"))
cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "lposc.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()
