baseline_PLL64 <- function(df_input) {
	return(df_input %>%
		filter(clock_source == "PLL64" & clock_freq %/% 1000000 == 64) %>%
		mutate(Source = "Baseline")
	)
}
baseline_PLL96 <- function(df_input) {
	return(df_input %>%
		filter(clock_source == "PLL96" & clock_freq %/% 1000000 == 96) %>%
		mutate(Source = "Baseline PLL96")
	)
}
baseline_RC_FAST <- function(df_input) {
	return(df_input %>%
		filter(clock_source == "RC_FAST" & clock_freq %/% 1000000 == 8) %>%
		mutate(Source = "Baseline RC_FAST")
	)
}
minimum_freq <- function(df_input) {
	return(df_input %>%
		filter(clock_source == "PLL64" & clock_freq %/% 1000000 == 1) %>% 
		mutate(Source = "Minimum frequency")
	)
}
minimum_freq_96 <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL96" & clock_freq %/% 1000000 == 1)) %>%
		mutate(Source = "Minimum frequency 96")
	)
}

get_combined_df <- function(df) {
	df1 <- baseline_PLL64(df)
	df2 <- baseline_PLL96(df)
	df3 <- baseline_RC_FAST(df)
	df4 <- minimum_freq(df)
	df5 <- minimum_freq_96(df)

	df1 <- df1 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df2 <- df2 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df3 <- df3 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df4 <- df4 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df5 <- df5 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()

	combined_df <- rbind(df1, df2, df3, df4, df5)

	combined_df$Source <- factor(combined_df$Source, 
															 levels = c("Baseline", 
																					"Baseline PLL96", 
																					"Baseline RC_FAST",
																					"Minimum frequency",
																					"Minimum frequency 96"))
	return(combined_df)
}

baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
baseline_name <- "PLL64.default.64MHz"
