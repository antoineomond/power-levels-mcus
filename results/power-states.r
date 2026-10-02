library("ggplot2")
library("dplyr")
library(rlang)
library(patchwork)
library(stringr)
library(tidyr)
library(grid)
library(gridExtra)
library(RColorBrewer)
options(dplyr.print_max = 1e9, pillar.width = Inf)

MHz <- 1000000
kHz <- 1000
args <- commandArgs(trailingOnly = TRUE)
folder <- args[1] 
graph_title <- args[2] 
target_folder <- ""

if(folder == "paper_esp32/") {
	source("filters-esp32.r")
	target_folder <- "esp32/"
}
if(folder == "paper_stm32/") {
	source("filters-stm32.r")
	target_folder <- "stm32/"
}
if(folder == "paper_pico2/") {
	source("filters-pico2.r")
	target_folder <- "pico2/"
}

# Assign parameters names according to experiment num
df <- read.csv(paste(folder, "results.csv", sep=""))
df <- df %>%
  mutate(config_row = expe_num + 1)
parameters <- read.csv(paste(folder, "configurations.csv", sep=""))
df <- df %>%
	left_join(
		parameters %>% mutate(config_row = row_number()),
		by = "config_row"
	) %>%
	select(-config_row)

combined_df <- get_combined_df(df)
selective_labeller <- function(value) {
  # If the value is in our hide-list, return an empty string
  #ifelse(value %in% c("Baseline", "Minimum frequency", "Minimum VREG", "Minimum freq & VREG"), as.character(value), "")
	value
}

get_plot <- function(df_expe) {
	# myColors <- c("PLL" = "black", "PLL64" = "black", "XOSC" = "blue", "ROSC" = "orange", "LPOSC" = "purple", "PLL96" = "blue", "HSI" = "blue", "RC_FAST" = "purple", "HSE" = "purple")
	myColors <- c("prime" = "black", "prime_multicores" = "grey", "mat_mul" = "cyan", "mat_mul_float" = "blue", "mat_mul_double" = "dark blue", "PLL96" = "blue", "HSI" = "blue", "RC_FAST" = "purple", "HSE" = "purple")
	mtimestamp <- max(df_expe$current_timestamp, na.rm = TRUE)
	p <- ggplot(df_expe , aes(x = current_timestamp, y = power_sample, color=benchmark_name, group=interaction(clock_source, vreg_output, clock_freq))) + 
		geom_line(na.rm = TRUE) +
		geom_hline(aes(yintercept = power_median), linetype = "dashed") +
		#geom_text(aes(x=mtimestamp*1.05, y = power_median, label = paste(round(power_median,2), "mW"))) +
		scale_x_continuous(expand = expansion(mult = c(0, 0.3))) +
		scale_y_continuous(n.breaks=5) +
		facet_wrap(~Source, ncol = 2, scales = "free", labeller = as_labeller(selective_labeller)) +
		labs(x = "Timestamp in seconds", y = "Power usage in mW", title = graph_title) +
		scale_colour_manual(name = "Benchmark name:", values = myColors) +
		guides(color = guide_legend(nrow = 1, byrow = TRUE)) +
		theme(
			legend.position = "top",
			plot.title = element_text(hjust = 0.5),
			plot.subtitle = element_text(hjust = 0.5),
			plot.margin = margin(0, 0, 0, 0, "pt")
		)
	return(p)
}
cp <- get_plot(combined_df)
pdf(paste("/home/aomond/research/mcu_sigmetrics27/images/", target_folder, "combined.pdf", sep=""))
print(cp)
dev.off()
