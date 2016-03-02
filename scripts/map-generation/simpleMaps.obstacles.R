library(raster)
library(sp)
library(rgdal)

centeredMap <- function( fileNameResource, fileNameObstacle, size, rangeValues, probaObstacle )
{
	minResource <- rangeValues[1]
	maxResource <- rangeValues[2]

	centerX <- size[1]/2
	centerY <- size[2]/2

	resources <- matrix(data=minResource,nrow=size[1],ncol=size[2])
	resources[centerX,centerY] <- maxResource

	resourcesRaster <- raster(resources)
	writeRaster(resourcesRaster, fileNameResource,"GTiff", datatype="INT4U", overwrite=TRUE)


	obstacles <- matrix(data=0,nrow=size[1],ncol=size[2])
	for(i in seq(1,size[1]))
	{
		for( j in seq(1,size[2]))
		{
			dice = runif(1,0,1)
			if (dice < probaObstacle)
			{
				obstacles[i,j] = 1
			}
		}
	}
	obstaclesRaster <- raster(obstacles)
	writeRaster(obstaclesRaster, fileNameObstacle,"GTiff", datatype="INT4U", overwrite=TRUE)

}

# example
proba = 0.1
path = "../../maps/"
fileNameResource = "map.tif"
fileNameObstacle = paste("obstacle_p" , proba, ".tif",sep="")
size = c(10,10)
rangeValues = c(1,10)

centeredMap(paste(path,fileNameResource,sep=""), paste(path,fileNameObstacle,sep=""), size, rangeValues, proba)
