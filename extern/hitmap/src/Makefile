#
# Hitmap 1.3
# Makefile adapted from template (see config directory)
#
# @author: Arturo Gonzalez-Escribano
# @date: Ago 2015
#

# <license>
# 
# Hitmap v1.4
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
# Copyright (c) 2007-2024, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>

#
# 1. DEFINITIONS AND FLAGS FOR THIS PARTICULAR APPLICATION
# 	(THEY WILL BE AUTOMATICALLY ADDED TO THE MAIN COMPILATION COMMAND)
#
APP_FLAGS=-funroll-loops -fPIC -g

#
# 2. DECLARE APPLICATION TARGETS
#
APP_TARGETS=../lib/libhit.a ../lib/libhit.so
APP_CLEAN=$(APP_TARGETS)

# ACTIVATE SPECIAL RULES TREATMENT FOR THE LIBRARY ITSELF
APP_HITMAP=yes
APP_OPENMP=yes
HIT_COMM=yes

#
# 4. LOAD Hitmap COMPILING DEFINITIONS
#	( PLEASE, SET HITMAP BASEDIR )
#
HIT_BASEDIR=..
include $(HIT_BASEDIR)/config/make.def

#
# 5. DECLARE TARGET SOURCES, AND SPECIFIC BUILDING RULES FOR YOUR APPLICATION
# 	( SUBSTITUTE THE EXAMPLES BELLOW )
#
INC=$(HIT_INCLUDE_DIR)

COMMON_OBJS= hit_sig.o \
	hit_shape.o hit_cshape.o hit_bshape.o \
	hit_domain.o \
	hit_tile.o hit_gctile.o hit_mctile.o hit_gbtile.o hit_mbtile.o \
	hit_tileFile.o \
	hit_blockTile.o \
	hit_topology.o \
	hit_layout.o hit_layoutMetis.o \
	hit_file.o  \
	hit_dump.o\
	hit_error.o\
	hit_env.o

OBJS= $(COMMON_OBJS)

ifeq ($(HIT_COMM), yes)
	OBJS+= hit_com.o hit_pattern.o 
endif

../lib/libhit.so: $(OBJS)
	$(CC) -shared -o $@ $(OBJS) $(HIT_EXT_METIS_LIB_DIRS) $(HIT_EXT_METIS_LIBS)

../lib/libhit.a: $(OBJS)
	ar -rcs $@ $(OBJS)

hit_sig.o: hit_sig.c $(INC)/hit_sig.h $(INC)/hit_error.h
	$(HIT_COMPILE_CC) -c $<

hit_shape.o: hit_shape.c $(INC)/hit_cshape.h $(INC)/hit_bshape.h $(INC)/hit_shape.h $(INC)/hit_sig.h $(INC)/hit_error.h $(INC)/hit_allocP.h
	$(HIT_COMPILE_CC) -c $<

hit_cshape.o: hit_cshape.c $(INC)/hit_cshape.h $(INC)/hit_shape.h $(INC)/hit_sig.h $(INC)/hit_error.h $(INC)/hit_allocP.h
	$(HIT_COMPILE_CC) -c $<

hit_bshape.o: hit_bshape.c $(INC)/hit_bshape.h $(INC)/hit_shape.h $(INC)/hit_sig.h $(INC)/hit_error.h $(INC)/hit_allocP.h
	$(HIT_COMPILE_CC) -c $<

hit_domain.o: hit_domain.c hit_shape.c $(INC)/hit_sshape.h $(INC)/hit_shape.h $(INC)/hit_sig.h $(INC)/hit_error.h $(INC)/hit_allocP.h
	$(HIT_COMPILE_CC) -c $<

hit_dump.o: hit_dump.c $(INC)/hit_dump.h
	$(HIT_COMPILE) -c $<

hit_tile.o: hit_tile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h
	$(HIT_COMPILE_CC) -c $<

hit_tileFile.o: hit_tileFile.c $(INC)/hit_tile.h $(INC)/hit_tileFile.h 
	$(HIT_COMPILE) -c $<

hit_gctile.o: hit_gctile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h $(INC)/hit_cshape.h
	$(HIT_COMPILE_CC) -c $<

hit_mctile.o: hit_mctile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h $(INC)/hit_cshape.h
	$(HIT_COMPILE_CC) -c $<

hit_gbtile.o: hit_gbtile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h $(INC)/hit_bshape.h
	$(HIT_COMPILE_CC) -c $<

hit_mbtile.o: hit_mbtile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h $(INC)/hit_bshape.h
	$(HIT_COMPILE_CC) -c $<

hit_blockTile.o: hit_blockTile.c $(INC)/hit_tile.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_shape.h $(INC)/hit_blockTile.h
	$(HIT_COMPILE) -c $<

hit_file.o: hit_file.c $(INC)/hit_file.h $(INC)/hit_tile.h $(INC)/hit_shape.h $(INC)/hit_cshape.h $(INC)/hit_bshape.h $(INC)/hit_error.h $(INC)/hit_allocP.h
	$(HIT_COMPILE) -c $<

hit_topology.o: hit_topology.c $(INC)/hit_topology.h $(INC)/hit_shape.h $(INC)/hit_error.h $(INC)/hit_com.h
	$(HIT_COMPILE) -c $<

hit_layout.o: hit_layout.c $(INC)/hit_layout.h $(INC)/hit_topology.h $(INC)/hit_shape.h $(INC)/hit_cshape.h $(INC)/hit_bshape.h $(INC)/hit_error.h $(INC)/hit_funcop.h
	$(HIT_COMPILE) -c $<

hit_layoutMetis.o: hit_layoutMetis.c $(INC)/hit_layout.h $(INC)/hit_layoutP.h $(INC)/hit_funcop.h $(INC)/hit_funcop.h $(INC)/hit_com.h $(INC)/hit_shape.h $(INC)/hit_cshape.h $(INC)/hit_bshape.h
	$(HIT_COMPILE) -c $<

hit_topContext.o: hit_topContext.c $(INC)/hit_topContext.h $(INC)/hit_layout.h $(INC)/hit_topology.h $(INC)/hit_error.h
	$(HIT_COMPILE) -c $<

hit_com.o: hit_com.c $(INC)/hit_com.h $(INC)/hit_error.h $(INC)/hit_allocP.h $(INC)/hit_tile.h $(INC)/hit_layout.h $(INC)/hit_funcop.h $(INC)/hit_shape.h $(INC)/hit_cshape.h $(INC)/hit_bshape.h
	$(HIT_COMPILE) -c $<

hit_pattern.o: hit_pattern.c $(INC)/hit_pattern.h $(INC)/hit_com.h
	$(HIT_COMPILE) -c $<

hit_error.o: hit_error.c $(INC)/hit_error.h
	$(HIT_COMPILE) -c $<

hit_env.o: hit_env.c $(INC)/hit_env.h
	$(HIT_COMPILE) -c $<

#
# END
#
