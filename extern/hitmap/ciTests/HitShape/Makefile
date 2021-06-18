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
APP_TARGETS= sshapeConstructor.test sshapeConstructorStd.test sshapeCmp.test \
	sshapeDimCard.test sshapeIntersect.test sshapeExpand.test sshapeBorder.test \
	sshapeSubset.test sshapeTransform.test sshapeTileToArray.test sshapeArrayToTile.test \
	sshapeIterator.test

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

sshapeConstructor.test: sshapeConstructorTest.c
sshapeConstructorStd.test: sshapeConstructorStdTest.c
sshapeCmp.test: sshapeCmpTest.c
sshapeDimCard.test: sshapeDimCardTest.c
sshapeIntersect.test: sshapeIntersectTest.c
sshapeExpand.test: sshapeExpandTest.c
sshapeBorder.test: sshapeBorderTest.c
sshapeSubset.test: sshapeSubsetTest.c
sshapeTileToArray.test: sshapeTileToArrayTest.c
sshapeArrayToTile.test: sshapeArrayToTileTest.c
sshapeTransform.test: sshapeTransformTest.c
sshapeIterator.test: sshapeIteratorTest.c

debugmode:
	make APP_FLAGS='-I.. -DDEBUG'
