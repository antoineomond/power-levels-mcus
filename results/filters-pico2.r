baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
no_filter_f <- function(df_input) {
	return(list(df_input, c(""), "result", baseline_mapfunc, "HSI | scale3 | 16MHz"))
}
baseline_pll <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 150)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "1.10V" & clock_freq %/% 1000 == 29)) %>%
		mutate(Source = "Baseline")
	)
}
baseline_xosc_rosc <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "XOSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 12)
				 | (clock_source == "ROSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 11)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "1.10V" & clock_freq %/% 1000 == 29)) %>%
		mutate(Source = "Baseline XOSC ROSC")
	)
}
minimum_freq_pll <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 15)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "1.10V" & clock_freq %/% 1000 == 21)) %>%
		mutate(Source = "Minimum frequency")
	)
}
minimum_freq_rosc <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "ROSC" & vreg_output == "1.10V" & clock_freq %/% 1000000 == 2)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "1.10V" & clock_freq %/% 1000 == 21)) %>%
		mutate(Source = "Minimum frequency ROSC")
	)
}
minimum_vreg_pll <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "0.90V" & clock_freq %/% 1000000 == 150)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "0.80V" & clock_freq %/% 1000 == 33)) %>%
		mutate(Source = "Minimum VREG")
	)
}
minimum_vreg_xosc_rosc <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "XOSC" & vreg_output == "0.80V" & clock_freq %/% 1000000 == 12)
				 | (clock_source == "ROSC" & vreg_output == "0.80V" & clock_freq %/% 1000000 == 3)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "0.80V" & clock_freq %/% 1000 == 33)) %>%
		mutate(Source = "Minimum VREG XOSC ROSC")
	)
}
minimum_freq_vreg_pll <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "0.90V" & clock_freq %/% 1000000 == 15)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "0.80V" & clock_freq %/% 1000 == 23)) %>%
		mutate(Source = "Minimum freq & VREG")
	)
}
minimum_freq_vreg_rosc <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "ROSC" & vreg_output == "0.80V" & clock_freq %/% 1000000 == 1)) %>%
				 # | (clock_source == "LPOSC" & vreg_output == "0.80V" & clock_freq %/% 1000 == 23)) %>%
		mutate(Source = "Minimum freq & VREG ROSC")
	)
}

get_combined_df <- function(df) {
	df1 <- baseline_pll(df)
	df2 <- baseline_xosc_rosc(df)
	df3 <- minimum_freq_pll(df)
	df4 <- minimum_freq_rosc(df)
	df5 <- minimum_vreg_pll(df)
	df6 <- minimum_vreg_xosc_rosc(df)
	df7 <- minimum_freq_vreg_pll(df)
	df8 <- minimum_freq_vreg_rosc(df)
	
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
																					"Minimum frequency", 
																					"Baseline XOSC ROSC", 
																					"Minimum frequency ROSC",
																					"Minimum VREG",
																					"Minimum freq & VREG",
																					"Minimum VREG XOSC ROSC", 
																					"Minimum freq & VREG ROSC"))
	return(combined_df)
}
baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
baseline_name <- "PLL | 1 | 10V | 150MHz"
