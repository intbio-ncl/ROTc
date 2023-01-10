library(xtable)

exhaustive_search <- function(data, labels, depth, threshold, genes) 
{
  wd <- getwd()
  run_data <- as.matrix(data)
  run_depth <- as.integer(depth)
  run_labels <- as.integer(labels)
  setwd("~/ROTc")
  dyn.load("lib_r_wrapper.so")
  return_val <- .Call("search", run_data, run_labels, run_depth, threshold, as.integer(1))
  return_val[,1:depth] <- genes[return_val[,1:depth]]
  setwd(wd)
  return(return_val)
}


setwd(".")

table <- read.delim("RNAseq_and_tiling_preprocessed", stringsAsFactors = FALSE)
data <- table[,3:ncol(table)]
stress_pos <- which(data[nrow(data),] =="STRESS")
data <- sapply(data[1:(nrow(data) - 1),], as.numeric)

loci <- table[,1]
genes <- table[,2]


labels <- rep(0, dim(data)[2])
labels[stress_pos] <- 1
colnames(data)[which(labels == 1)]

dir.create("Results")
individual <- exhaustive_search(data, labels, 1, c(0), genes)

colnames(individual) <- c("Gene 1", "Margin 1", "Threshold 1")
write.table(individual, "Results/individual_ranking.csv", sep="\t", row.names = FALSE)
print(xtable(individual[1:min(10, dim(individual)[1]),], type = "latex"), file = "Results/individuals.tex")

pairs<- exhaustive_search(data, labels, 2, c(0), genes)

colnames(pairs) <- c("Gene 1", "Gene 2","Margin 1", "Margin 2", "Threshold 1", "Threshold 2")
write.table(pairs, "Results/pair_ranking.csv", sep="\t", row.names = FALSE)
print(xtable(pairs[1:10,], type = "latex"), file = "Results/pairs.tex")

thresholds <- sort(as.numeric(pairs[1,c(3,4)]))[1:100]

triples <- exhaustive_search(data, labels, 3, thresholds, genes)

colnames(triples) <- c("Gene 1", "Gene 2", "Gene 3", 
                       "Margin 1", "Margin 2", "Margin 3", 
                       "Threshold 1", "Threshold 2", "Threshold 3")
write.table(triples, "Results/triple_ranking.csv", sep="\t", row.names = FALSE)
print(xtable(triples[1:10,], type = "latex"), file = "Results/triples.tex")
