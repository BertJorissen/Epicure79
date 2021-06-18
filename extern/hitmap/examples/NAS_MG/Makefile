#
# Hitmap 1.2
# Makefile for MG NAS Benchmark
#
#
# (c) 2011-2015, Arturo Gonzalez-Escribano
#

# <license>
# 
# Hitmap v1.3
# 
# This software is provided to enhance knowledge and encourage progress in the scientific
# community. It should be used only for research and educational purposes. Any reproduction
# or use for commercial purpose, public redistribution, in source or binary forms, with or 
# without modifications, is NOT ALLOWED without the previous authorization of the copyright 
# holder. The origin of this software must not be misrepresented; you must not claim that you
# wrote the original software. If you use this software for any purpose (e.g. publication),
# a reference to the software package and the authors must be included.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# Copyright (c) 2007-2019, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>

#
# 1. DEFINITIONS AND FLAGS FOR THIS PARTICULAR APPLICATION
# 	(THEY WILL BE AUTOMATICALLY ADDED TO THE MAIN COMPILATION COMMAND)
#
APP_FLAGS=
APP_LIBS=
APP_TESTING=

#
# 2. DECLARE APPLICATION TARGETS,
# 	 AND OTHER FILES TO BE AUTOMATICALLY REMOVED WHEN CLEANING
#
APP_TARGETS=mg_classA
APP_CLEAN=mg_classS mg_classA mg_classB mg_classC mg_classD mg_classE

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
mg_classS: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=S $^ $(HIT_LIBS) -o $@

mg_classA: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=A $^ $(HIT_LIBS) -o $@

mg_classB: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=B $^ $(HIT_LIBS) -o $@

mg_classC: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=C $^ $(HIT_LIBS) -o $@

mg_classD: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=D $^ $(HIT_LIBS) -o $@

mg_classE: mg.c c_randi8.c
	$(HIT_COMPILE) -DCLASS=E $^ $(HIT_LIBS) -o $@

allClasses: mg_classS mg_classA mg_classB mg_classC mg_classD mg_classE

.PHONY: allClasses
