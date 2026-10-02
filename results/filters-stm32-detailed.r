baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
no_filter_f <- function(df_input) {
	return(list(df_input, c(""), "result", baseline_mapfunc, "HSI | scale3 | 16MHz"))
}
baseline_HSI <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSI" & vreg_output == "scale3" & clock_freq %/% 1000000 == 16)) %>%
		mutate(Source = "Baseline")
	)
}
baseline_HSE <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSE" & vreg_output == "scale3" & clock_freq %/% 1000000 == 25)) %>%
		mutate(Source = "Baseline HSE")
	)
}
baseline_PLL <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale1" & clock_freq %/% 1000000 == 64)) %>%
		mutate(Source = "Baseline PLL")
	)
}
minimum_freq_HSI <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSI" & vreg_output == "scale3" & clock_freq %/% 1000000 == 1)) %>%
		mutate(Source = "Minimum frequency")
	)
}
minimum_freq_HSE <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSE" & vreg_output == "scale3" & clock_freq %/% 1000000 == 1)) %>%
		mutate(Source = "Minimum frequency HSE")
	)
}
minimum_freq_PLL <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale1" & clock_freq %/% 1000000 == 1)) %>%
		mutate(Source = "Minimum frequency PLL")
	)
}
minimum_vreg <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale3" & clock_freq %/% 1000000 == 64)) %>%
		mutate(Source = "Minimum VREG")
	)
}
minimum_freq_vreg <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale3" & clock_freq %/% 1000000 == 1)) %>%
		mutate(Source = "Minimum freq & VREG")
	)
}

get_combined_df <- function(df) {
	df1 <- baseline_HSI(df)
	df2 <- baseline_HSE(df)
	df3 <- baseline_PLL(df)
	df4 <- minimum_freq_HSI(df)
	df5 <- minimum_freq_HSE(df)
	df6 <- minimum_freq_PLL(df)
	df7 <- minimum_vreg(df)
	df8 <- minimum_freq_vreg(df)
	
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
	df6 <- df6 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df7 <- df7 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()
	df8 <- df8 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()

	combined_df <- rbind(df1, df2, df3, df4, df5, df6, df7, df8)

	combined_df$Source <- factor(combined_df$Source, 
															 levels = c("Baseline", 
																					"Baseline HSE", 
																					"Baseline PLL",
																					"Minimum frequency",
																					"Minimum frequency HSE",
																					"Minimum frequency PLL",
																					"Minimum VREG", 
																					"Minimum freq & VREG"))
	return(combined_df)
}
baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
baseline_name <- "HSI | scale3 | 16MHz"
