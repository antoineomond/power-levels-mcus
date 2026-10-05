source("power-states.r")

cp <- get_plot(combined_df)
pdf(paste(folder, "combined.pdf", sep=""))
print(cp)
dev.off()


