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

APP_TARGETS= sigCmp.test sigIn.test sigTileToArray.test sigIntersect.test \
	sigBlend.test sigExpand.test

APP_FLAGS= -I..
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
include $(HIT_BASEDIR)/config/make.def

#
# 5. DECLARE TARGET SOURCES, AND SPECIFIC BUILDING RULES FOR YOUR APPLICATION
# 	( SUBSTITUTE THE EXAMPLES BELLOW )
#

sigCmp.test: sigCmpTest.c
sigIn.test: sigInTest.c
sigTileToArray.test: sigTileToArrayTest.c
sigIntersect.test: sigIntersectTest.c
sigBlend.test: sigBlendTest.c
sigExpand.test: sigExpandTest.c

debugmode:
	make APP_FLAGS='-I.. -DDEBUG'
