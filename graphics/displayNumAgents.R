#!/usr/bin/Rscript

require("graphics")
library("plyr")


args <- commandArgs(trailingOnly = TRUE)

input <- args[1]
directories <- Sys.glob(paste(input,"run_*",sep="/"))
type <- args[2]
output <- args[3]


#store each csv file in a big data frame
file <- paste("agent-",type,".csv",sep="")
data <- read.csv(paste(directories[1],"csv",file,sep="/"),sep=";")
data <- cbind(data,rep("1",nrow(data)))
bigData <- data
colnames(bigData) <- c("timeStep","nbAgents","meanResources","expe")

for (i in seq(2,length(directories),1))
{
	directory <- directories[i]
	data <- read.csv(paste(directory,"csv",file,sep="/"),sep=";")
	data <- cbind(data,rep(toString(i),nrow(data))) 
	colnames(data) <- c("timeStep","nbAgents","meanResources","expe")
	bigData <- rbind(bigData,data)
}


#compute the mean for each timeStep
mean <- ldply(
							.data=unique(bigData$timeStep),
							.fun = function(x){
								mean(bigData[bigData$timeStep == x,]$nbAgents)
							}
							)

#compute the standard dev for each timeStep
stdDev <- ldply(
							.data=unique(bigData$timeStep),
							.fun = function(x){
								sd(bigData[bigData$timeStep == x,]$nbAgents)
							}
							)

#compute the lines and store them
meanPlus <- mean + stdDev
meanMinus <- mean - stdDev
analysed <- cbind(unique(bigData$timeStep),mean,meanPlus,meanMinus)
colnames(analysed) <- c("timeStep","mean","meanPlus","meanMinus")

setEPS()
postscript(output)
plot(analysed$timeStep,analysed$mean,type="l",ylim=c(0,2300))
lines(analysed$timeStep,analysed$meanMinus)
lines(analysed$timeStep,analysed$meanPlus)
dev.off()
