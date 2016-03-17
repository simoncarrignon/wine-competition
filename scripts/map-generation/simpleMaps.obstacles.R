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
			if ((i != centerX) && (j!= centerY))
			{
				dice = runif(1,0,1)
				if (dice < probaObstacle)
				{
					obstacles[i,j] = 1
				}
			}
		}
	}
	obstaclesRaster <- raster(obstacles)
	writeRaster(obstaclesRaster, fileNameObstacle,"GTiff", datatype="INT4U", overwrite=TRUE)

}

# example
for (size in c(10,50))
{
	for (proba in seq(0.0,0.9,0.1))
	{
		width=size
		height=size
		path = "../../maps/"
		fileNameResource = paste("resourceCenter_s",width,"x",height,".tif",sep="")
		fileNameObstacle = paste("obstacle_p" , proba, "_s", width , "x", height, ".tif",sep="")

		rangeValues = c(1,10)
		centeredMap(paste(path,fileNameResource,sep=""), paste(path,fileNameObstacle,sep=""), c(width,height), rangeValues, proba)
	}
}
