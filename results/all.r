source("power-states.r")

cp <- get_plot(combined_df)
n_facets <- nrow(distinct(combined_df, clock_source, vreg_output, clock_freq))
pdf(paste(folder, "combined.pdf", sep=""), width = 4, height = 2.5*n_facets)
print(cp)
dev.off()


