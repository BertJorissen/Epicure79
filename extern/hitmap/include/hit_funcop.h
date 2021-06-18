/**
 * Simple extra functions and operators.
 * Extra functions and operators to transform indexes and shape boundaries.
 * 
 * @file hit_funcop.h
 * @version 0.9.2
 * @author Arturo Gonzalez-Escribano
 * @author Javier Fresno Bausela
 * @author Carlos de Blas Carton
 * @date Jun 2010 
*/

/*
 * <license>
 * 
 * Hitmap v1.3
 * 
 * This software is provided to enhance knowledge and encourage progress in the scientific
 * community. It should be used only for research and educational purposes. Any reproduction
 * or use for commercial purpose, public redistribution, in source or binary forms, with or 
 * without modifications, is NOT ALLOWED without the previous authorization of the copyright 
 * holder. The origin of this software must not be misrepresented; you must not claim that you
 * wrote the original software. If you use this software for any purpose (e.g. publication),
 * a reference to the software package and the authors must be included.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Copyright (c) 2007-2021, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 * 
 * More information on http://trasgo.infor.uva.es/
 * 
 * </license>
*/

#ifndef _HitFuncOp_
#define _HitFuncOp_

/* 1. INTEGER MAX,MIN FUNCTIONS */
/**
 * By default: Use macro versions of operators
 */
#define HIT_USE_MACRO_OP

#ifdef HIT_USE_MACRO_OP
/**
 * Maximum of two numbers (macro version).
 * @param a Number of any type
 * @param b Number of any type
 */
#define hit_max(a,b)	(((a)>(b)) ? (a) : (b))
/**
 * Minimum of two numbers (macro version).
 * @param a Number of any type
 * @param b Number of any type
 */
#define hit_min(a,b)	(((a)<(b)) ? (a) : (b))
#else
/**
 * Maximum of two numbers (inlined function version).
 * @param a Number of any type
 * @param b Number of any type
 */
static inline int hit_max(int a, int b) { return (a>b) ? a:b; }


/**
 * Minimum of two numbers (inlined function version).
 * @param a Number of any type
 * @param b Number of any type
 */
static inline int hit_min(int a, int b) { return (a>b) ? b:a; }
#endif


/* END OF HEADER FILE _HitFuncOp_ */
#endif
