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


combined_df <- df

# get the last 20 iterations
combined_df <- combined_df %>%
	filter(iteration_num < 20)

# discard the n first and last results for each benchmark 
nb_discards <- 10
combined_df <- combined_df %>%
	group_by(iteration_num, expe_num) %>%
	filter(between(row_number(), nb_discards, n()-nb_discards)) %>%
	ungroup()

# compute power outside to prevent duplicate values
pwr <- combined_df %>%
	group_by(clock_source, vreg_output, clock_freq) %>%
	summarise(power_median = median(current_sample, na.rm = TRUE), mtimestamp = max(current_timestamp, na.rm = TRUE)) %>%
	ungroup()

selective_labeller <- function(value) {
	if(grepl("stm", folder)) {
		labels <- stringr::str_split_fixed(value, "\\.", 3)
		freq_mhz <- as.numeric(labels[, 3])
		freq_string <- ifelse(
			freq_mhz/1000000 < 1,
			paste0(round(freq_mhz/1000, 2), "kHz"),
			paste0(round(freq_mhz/1000000, 2), "MHz")
		)
		paste0("Clk source: ", labels[, 1], ", Freq: ", freq_string, ", VREG: ", labels[, 2])
	} else {
		labels <- stringr::str_split_fixed(value, "\\.", 4)
		freq_mhz <- as.numeric(labels[, 4])
		freq_string <- ifelse(
			freq_mhz/1000000 < 1,
			paste0(round(freq_mhz/1000, 2), "kHz"),
			paste0(round(freq_mhz/1000000, 2), "MHz")
		)
		paste0("Clk source: ", labels[, 1], ", Freq: ", freq_string, ", VREG: ", labels[, 2], ".", labels[, 3])
	}
}


get_plot <- function(df_expe) {
	# myColors <- c("PLL" = "black", "PLL64" = "black", "XOSC" = "blue", "ROSC" = "orange", "LPOSC" = "purple", "PLL96" = "blue", "HSI" = "blue", "RC_FAST" = "purple", "HSE" = "purple")
	myColors <- c("prime" = "black", "prime_multicores" = "grey", "mat_mul" = "cyan", "mat_mul_float" = "blue", "mat_mul_double" = "dark blue", "PLL96" = "blue", "HSI" = "blue", "RC_FAST" = "purple", "HSE" = "purple")
	p <- ggplot(df_expe , aes(x = current_timestamp, y = current_sample, color=benchmark_name, group=interaction(clock_source, vreg_output, clock_freq))) + 
		geom_line(na.rm = TRUE) +
		geom_hline(data=pwr, aes(yintercept = power_median), linetype = "dashed") +
		geom_label(data=pwr, aes(x=Inf, y = power_median, label = paste(round(power_median,2), "mA")), inherit.aes = FALSE, show.legend = FALSE, hjust=1.00) +
		scale_x_continuous(expand = expansion(mult = c(0, 0.4))) +
		scale_y_continuous(n.breaks=5) +
		facet_wrap(~interaction(clock_source, vreg_output, clock_freq), ncol = 2, scales = "free", labeller = as_labeller(selective_labeller)) +
		labs(x = "Timestamp in seconds", y = "Power usage in mW", title = "") +
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
pdf(paste(folder, "combined.pdf", sep=""))
print(cp)
dev.off()
