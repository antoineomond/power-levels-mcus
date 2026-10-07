source("power-states.r")
legend <- cowplot::get_legend(get_plot(combined_df))

grid.newpage()
grid.draw(legend)
