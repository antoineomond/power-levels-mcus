library("ggplot2")
library("ggrepel")
library("dplyr")
library("ggtext")
library(rlang)
library(patchwork)
library(stringr)
library(tidyr)
library(grid)
library(gridExtra)
library(RColorBrewer)
library(cowplot)
library(viridis)
#options(dplyr.print_max = 1e9, pillar.width = Inf)

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


selective_labeller <- function(value) {
	labels <- stringr::str_split_fixed(value, "\\|", 3)
	freq_mhz <- as.numeric(labels[, 3])
	freq_string <- ifelse(
		freq_mhz/1000000 < 1,
		paste0(round(freq_mhz/1000, 2), "kHz"),
		paste0(round(freq_mhz/1000000, 2), "MHz")
	)
	paste0("Clk source: **", labels[, 1], "**, Freq: **", freq_string, "**, VREG: **", labels[, 2], "**")
}

get_plot <- function(df_expe) {
	# compute power outside to prevent duplicate values
	pwr <- df_expe %>%
		group_by(clock_source, vreg_output, clock_freq, benchmark_name) %>%
		summarise(power_median = median(power_sample, na.rm = TRUE), mtimestamp = max(current_timestamp, na.rm = TRUE)) %>%
		ungroup()
	df_expe <- df_expe %>%
		mutate(
			clock_source = factor(clock_source, levels = c("HSI", "HSE", "PLL", "XOSC", "ROSC", "LPOSC", "PLL96", "PLL64", "RC_FAST", "XTAL")),
			vreg_output = factor(vreg_output, levels = c("1.10V", "1.00V", "0.90V","0.85V", "0.80V", "Range1", "Range2", "Range3"))
		)
	bench <- c("prime", "prime_multicores", "mat_mul", "mat_mul_float", "mat_mul_double")
	myColors <- setNames(viridisLite::viridis(length(bench), end = 0.9), bench)
	# myColors <- setNames(c("#000000", "#E69F00", "#0072B2", "#009E73", "#D55E00"), bench)
	p <- ggplot(df_expe , aes(x = current_timestamp, y = power_sample, color=benchmark_name, group=interaction(clock_source, vreg_output, clock_freq, benchmark_name))) +
		geom_line(na.rm = TRUE, key_glyph = "rect") +
		geom_hline(data=pwr, aes(yintercept = power_median, color = benchmark_name), linetype = "dashed", linewidth = 0.3, alpha = 0.5, show.legend = FALSE) +
		geom_label_repel(
			data=pwr,
			aes(x=Inf, y = power_median, color = benchmark_name, label = sprintf("%.2f mW", power_median)),
			inherit.aes = FALSE, show.legend = FALSE, hjust=1.15,
			direction = "y",
			point.size = NA,
			min.segment.length = Inf,
			box.padding = 0.1, force_pull = 10, seed = 42
		) +
		scale_x_continuous(expand = expansion(mult = c(0, 0.4))) +
		scale_y_continuous(n.breaks=5) +
		facet_wrap(~interaction(clock_source, vreg_output, clock_freq, sep="|", lex.order = TRUE), ncol = 1, scales = "free", labeller = as_labeller(selective_labeller)) +
		labs(x = "Timestamp in seconds", y = "Power usage in mW", title = "") +
		scale_colour_manual(name = "Benchmark name:", values = myColors, breaks=names(myColors), labels = function(x) gsub("_", " ", x)) +
		guides(color = guide_legend(nrow = 1, byrow = TRUE, title.position = "left", title.hjust = 0.5)) +
		theme_bw() +
		theme(
			legend.position = "none",
			strip.text = ggtext::element_markdown(size=10),
			plot.title = element_text(hjust = 0.5),
			plot.subtitle = element_text(hjust = 0.5),
			plot.margin = margin(0, 0, 0, 0, "pt")
		)
	return(p)
}
