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

APP_TARGETS= laySig.test plug_layCopy.test layTopology.test layTopology2.test \
			plug_layDimBlocks.test plug_layBlocks.test plug_layMinBlocks.test \
			layMinShape.test hitWeights.test plug_layInLeader.test layNeighborUnwrapped.test \
			layNeighborUnwrapped2.test layNeighborWrapped.test layNeighborWrapped2.test \
			layShapeNeighbor.test layWrapNeighborsDim.test laySelfRanks.test layMaxShape.test \
			layIterator.test plug_layContiguous.test plug_layIndependentLB.test \
			layListBasics.test layBasics.test layProcGroup.test

APP_FLAGS=-I.. -g
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

laySig.test: laySigTest.c
plug_layCopy.test: plug_layCopyTest.c
plug_layDimBlocks.test: plug_layDimBlocksTest.c
plug_layBlocks.test: plug_layBlocksTest.c
plug_layMinBlocks.test: plug_layMinBlocksTest.c
plug_layInLeader.test: plug_layInLeaderTest.c
plug_layContiguous.test: plug_layContiguousTest.c
plug_layIndependentLB.test: plug_layIndependentLBTest.c
layTopology.test: layTopologyTest.c
layTopology2.test: layTopology2Test.c
layMinShape.test: layMinShapeTest.c
layMaxShape.test: layMaxShapeTest.c
hitWeights.test: hitWeightsTest.c
layNeighborUnwrapped.test: layNeighborUnwrappedTest.c	#
layNeighborUnwrapped2.test: layNeighborUnwrapped2Test.c	##
layNeighborWrapped.test: layNeighborWrappedTest.c		#
layNeighborWrapped2.test: layNeighborWrapped2Test.c		##
layShapeNeighbor.test: layShapeNeighborTest.c
layWrapNeighborsDim.test: layWrapNeighborsDimTest.c
laySelfRanks.test: laySelfRanksTest.c
layIterator.test:  layIteratorTest.c
layListBasics.test: layListBasicsTest.c
layBasics.test: layBasicsTest.c
layProcGroup.test: layProcGroupTest.c

debugmode:
	make APP_FLAGS='-I.. -DDEBUG -lm -g'
