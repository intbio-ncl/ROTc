table <- read.delim("pair_ranking.csv", stringsAsFactors = FALSE)
thresholds <- sort(as.numeric(pairs[1,c(3,4)]))
thresholds