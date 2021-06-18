#
# Hitmap 1.1
# Makefile template for examples
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
# 1. DEFINITIONS, FLAGS, AND EXTERNAL LIBRARIES FOR THIS PARTICULAR APPLICATION
# 	(THEY WILL BE AUTOMATICALLY ADDED TO THE MAIN COMPILATION COMMAND)
#
APP_FLAGS=
APP_LIBS=

#
# 2. DECLARE APPLICATION TARGETS
#
APP_TARGETS= SWseq_ref SWpar_ref SWseq SWpar

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


SWseq: SWseq.c SWcommon.c SWcommon.h SWcommon_hit.h SWcommon_hit.c
	$(HIT_COMPILE) -DHITSEQ $< SWcommon.c SWcommon_hit.c $(HIT_LIBS) -o $@

SWpar: SWpar.c SWcommon.c SWcommon.h SWcommon_hit.h SWcommon_hit.c
	$(HIT_COMPILE) -DHITPAR $< SWcommon.c SWcommon_hit.c $(HIT_LIBS) -o $@


#
# 7. REFERENCE CODES
#
SWseq_ref: SWseq_ref.c SWcommon.h SWcommon.c SWcommon_ref.h SWcommon_ref.c
	$(HIT_COMPILE_CC) -DREFSEQ $< SWcommon.c SWcommon_ref.c -o $@

SWpar_ref: SWpar_ref.c SWcommon.c SWcommon.h SWcommon_ref.h SWcommon_ref.c
	$(HIT_COMPILE) -DREFPAR $< SWcommon.c SWcommon_ref.c  -o $@


#
# END
#
