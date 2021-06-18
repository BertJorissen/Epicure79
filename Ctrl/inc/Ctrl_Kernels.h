#ifndef _CTRL_KERNELS_H_
#define _CTRL_KERNELS_H_
/**
 * @file Ctrl_Kernels.h
 * @author Trasgo Group
 * @brief Controllers include file for kernel files.
 * @version 2.1
 * @date 2021-04-26
 * 
 * @copyright This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * @copyright THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * @copyright Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * @copyright More information on http://trasgo.infor.uva.es/ */
#include "hitmap2.h"
#include "Core/Ctrl_KHitTile.h"
#include "Kernel/Ctrl_KernelProto.h"
/**
 * Generate particular polymorphic types of HitTile and KHitTile.
 *
 * This macro allows to define particular polymorphic types of HitTile. The new defined type
 * will be named: \e HitTile_\<baseType\>.
 *
 * @hideinitializer
 * 
 * @param type Name of a valid native or derived C type.
 */
#define Ctrl_NewType( type ) \
	typedef struct { \
		type *data; \
		int origAcumCard[4]; \
		int card[3]; \
		KHitShape shape; \
	} KHitTile_##type;

#endif // _CTRL_KERNELS_H_

