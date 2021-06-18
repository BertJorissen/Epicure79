#
# Hitmap 1.1
# Makefile template for examples
#

#
# 1. DEFINITIONS AND FLAGS FOR THIS PARTICULAR APPLICATION
# 	(THEY WILL BE AUTOMATICALLY ADDED TO THE MAIN COMPILATION COMMAND)
#

#
# 2. DECLARE APPLICATION TARGETS
#
CC=gcc

APP_TARGETS= tileShape.test tileSig.test tileHasArrayCoord.test tileCard.test \
			tileTile2Array.test tileConstructor.test tileRoot.test tileUpdate.test \
			tileIterator.test tileClone.test tileCheckBoundary.test tileElemAt.test \
			tileElemAtNoStride.test tileElemAtArrayCoords.test tileSelect.test \
			tileSelectArrayCoords.test tileShapeTile2Array.test

APP_FLAGS=-I..
#
# 3. DOES THIS APPLICATION CALL PLUGINS WHICH USE LIBRARIES EXTERNAL TO HITMAP ?
#	( UNCOMMENT IF NEEDED )
#
//HIT_USE_METIS=n

#
# 4. LOAD Hitmap COMPILING DEFINITIONS 
#	( PLEASE, SET HITMAP BASEDIR )
#
HIT_BASEDIR=../..
#HIT_BASEDIR=/home/mariasg/workspace/TFG/hitmapMaster/hitmap
include $(HIT_BASEDIR)/config/make.def

#
# 5. DECLARE TARGET SOURCES, AND SPECIFIC BUILDING RULES FOR YOUR APPLICATION
# 	( SUBSTITUTE THE EXAMPLES BELLOW )
#

tileShape.test: tileShapeTest.c
tileSig.test: tileSigTest.c
tileHasArrayCoord.test: tileHasArrayCoordTest.c
tileIterator.test: tileIteratorTest.c
tileClone.test: tileCloneTest.c
tileCard.test: tileCardTest.c
tileTile2Array.test: tileTile2ArrayTest.c
tileConstructor.test: tileConstructorTest.c
tileRoot.test: tileRootTest.c
tileUpdate.test: tileUpdateTest.c
tileCheckBoundary.test: tileCheckBoundaryTest.c
tileElemAt.test: tileElemAtTest.c
tileElemAtNoStride.test: tileElemAtNoStrideTest.c
tileElemAtArrayCoords.test: tileElemAtArrayCoordsTest.c
tileSelect.test: tileSelectTest.c
tileSelectArrayCoords.test: tileSelectArrayCoordsTest.c
tileShapeTile2Array.test: tileShapeTile2ArrayTest.c

debugmode:
	make APP_FLAGS='-I.. -DDEBUG'
