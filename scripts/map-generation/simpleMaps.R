library(raster)
library(sp)
library(rgdal)

centeredMap <- function( fileName, size, rangeValues )
{
	minResource <- rangeValues[1]
	maxResource <- rangeValues[2]

	centerX <- size[1]/2
	centerY <- size[2]/2

	resources <- matrix(data=minResource,nrow=size[1],ncol=size[2])
	resources[centerX,centerY] <- maxResource

	resourcesRaster <- raster(resources)
	writeRaster(resourcesRaster, fileName,"GTiff", datatype="INT4U", overwrite=TRUE)
}

# example
path = "../../maps/"
fileName = "map.tif"
size = c(10,10)
rangeValues = c(1,10)

centeredMap(paste(path,fileName,sep=""), size, rangeValues)
