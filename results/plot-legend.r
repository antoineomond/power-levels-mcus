source("power-states.r")
p <- get_plot(combined_df) + theme(legend.position = "top")
legend <- cowplot::get_plot_component(p, "guide-box-top")

w <- grid::convertWidth(sum(legend$widths), "in", valueOnly = TRUE)
h <- grid::convertHeight(sum(legend$heights), "in", valueOnly = TRUE)
pdf("legend.pdf", width = w, height = h)
grid::grid.draw(legend)
dev.off()
