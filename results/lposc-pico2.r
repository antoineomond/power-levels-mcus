source("power-states.r")

combined_df <- combined_df %>%
		filter((clock_source == "LPOSC" & vreg_output == "1.10V") |
		(clock_source == "LPOSC" & vreg_output == "0.80V"))
cp <- get_plot(combined_df)
pdf(paste(folder, "lposc.pdf", sep=""))
print(cp)
dev.off()
