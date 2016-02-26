#!/usr/bin/Rscript

require("graphics")
library("plyr")


args <- commandArgs(trailingOnly = TRUE)

nbAgentTypes <- as.integer(args[1])
argIt = 2
inputInfo <- matrix(data=NA, nrow=nbAgentTypes, ncol =2)
for( i in seq(1,nbAgentTypes))
{
	input <- args[argIt]
	argIt = argIt + 1
	inputInfo[i,1] <- input

	type <- args[argIt]
	argIt = argIt + 1
	inputInfo[i,2] <- type
}
output <- args[argIt]

setEPS()
postscript(output)
#create an empty plot
plot(0,ylim=c(0,2300),xlim=c(0,2000))
labelLine = c()
graphColors <- topo.colors(nbAgentTypes)

for( i in seq(1,nbAgentTypes))
{
	input <- inputInfo[i,1]
	type <- inputInfo[i,2]
	directories <- Sys.glob(paste(input,"run_*",sep="/"))

	labelLine <- c(labelLine,type)

	#store each csv file in a big data frame
	file <- paste("agent-",type,".csv",sep="")
	data <- read.csv(paste(directories[1],"csv",file,sep="/"),sep=";")
	data <- cbind(data,rep("1",nrow(data)))
	bigData <- data
	colnames(bigData) <- c("timeStep","nbAgents","meanResources","expe")

	for (j in seq(2,length(directories),1))
	{
		directory <- directories[j]
		data <- read.csv(paste(directory,"csv",file,sep="/"),sep=";")
		data <- cbind(data,rep(toString(j),nrow(data))) 
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

	lines(analysed$timeStep,analysed$meanMinus,col=graphColors[i])
	lines(analysed$timeStep,analysed$meanPlus,col=graphColors[i])
}

legend(x="topright",legend=labelLine,col=graphColors,lwd=1, lty=1)

dev.off()
