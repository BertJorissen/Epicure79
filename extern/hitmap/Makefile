#
# Hit v1.3
#
# Makefile Rev.6
#
# (c) 2010-2021, Arturo Gonzalez Escribano, Javier Fresno Bausela
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
# Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
# All rights reserved.
# 
# More information on http://trasgo.infor.uva.es/
# 
# </license>


VERSION=v1.3

all:
	if [ ! -d "lib" ] ; then mkdir lib ; fi
	make -C extern
	make -C src

clean:
	make -C extern clean
	make -C src clean
	make cleanexamples
	rm -rf lib/

cleanexamples:
	for example in examples/*; do if [ -d $$example ]; then make -C $$example clean || break; fi; done;

examples:
	for example in examples/*; do if [ -d $$example ]; then make -C $$example || break; fi; done

examplescheck:
	for example in examples/*; do if [ -d $$example ]; then make check -C $$example || break; fi; done

.PHONY: examples cleanexamples
