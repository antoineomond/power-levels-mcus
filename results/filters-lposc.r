baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
no_filter_f <- function(df_input) {
	return(list(df_input, c(""), "result", baseline_mapfunc, "HSI | scale3 | 16MHz"))
}
lposc_f <- function(df_input) {
	return(df_input %>%
		filter((clock_source == "LPOSC" & vreg_output == "1.10V" & clock_freq %/% 1000 == 29)
				 | (clock_source == "LPOSC" & vreg_output == "0.80V" & clock_freq %/% 1000 == 33)) %>%
		mutate(Source = "LPOSC")
	)
}

get_combined_df <- function(df) {
	df1 <- lposc_f(df)

	df1 <- df1 %>%
		group_by(clock_source, vreg_output, clock_freq) %>%
		mutate(power_median = median(power_sample, na.rm = TRUE)) %>%
		ungroup()

	combined_df <- rbind(df1)

	combined_df$Source <- factor(combined_df$Source, 
															 levels = c("LPOSC"))
	return(combined_df)
}
baseline_mapfunc <- function(lvls) { return(gsub("(C|V|L|z|I|1|2|3)\\.", "\\1 | ", lvls)) }
baseline_name <- "LPOSC | 1 | 10V | 33kHz"

