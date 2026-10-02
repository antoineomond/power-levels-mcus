baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
no_filter_f <- function(df_input) {
	return(list(df_input, c(""), "result", baseline_mapfunc, "HSI | scale3 | 16MHz"))
}
baseline_f <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSI" & vreg_output == "scale3") 
				 | (clock_source == "HSE" & vreg_output == "scale3")
				 | (clock_source == "PLL" & vreg_output == "scale1")) %>%
		filter(clock_freq %/% 1000000 %in% c(64, 25, 16))%>%
		mutate(Source = "Baseline")
	)
}
minimum_freq <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "HSI" & vreg_output == "scale3") 
				 | (clock_source == "HSE" & vreg_output == "scale3")
				 | (clock_source == "PLL" & vreg_output == "scale1")) %>%
		filter(clock_freq %/% 1000000 %in% c(1))%>%
		mutate(Source = "Minimum frequency")
	)
}
minimum_vreg_64 <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale1") 
				 | (clock_source == "PLL" & vreg_output == "scale3")) %>% 
		filter(clock_freq %/% 1000000 %in% c(64))%>%
		mutate(Source = "Minimum VREG 64MHz")
	)
}
minimum_vreg_1 <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "PLL" & vreg_output == "scale1") 
				 | (clock_source == "PLL" & vreg_output == "scale3")) %>% 
		filter(clock_freq %/% 1000000 %in% c(1)) %>%
		mutate(Source = "Minimum VREG 1MHz")
	)
}

get_combined_df <- function(df) {
	df1 <- baseline_f(df)
	df2 <- minimum_freq(df)
	df3 <- minimum_vreg_64(df)
	df4 <- minimum_vreg_1(df)

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

	combined_df <- rbind(df1, df2, df3, df4)

	combined_df$Source <- factor(combined_df$Source, 
															 levels = c("Baseline", 
																					"Minimum frequency", 
																					"Minimum VREG 64MHz",
																					"Minimum VREG 1MHz"))
	return(combined_df)
}
baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
baseline_name <- "HSI | scale3 | 16MHz"
