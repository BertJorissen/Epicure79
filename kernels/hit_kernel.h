/*
 * <license>
 *
 * Controller v4.0
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
 * Copyright (c) 2007-2020, Trasgo Group, Universidad de Valladolid.
 * All rights reserved.
 *
 * More information on http://trasgo.infor.uva.es/
 *
 * </license>
 */
/* From hitmap2.h */
#define HIT2_COUNTPARAM(...) HIT2_COUNTPARAM_N(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define HIT2_COUNTPARAM_N(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, num, ...) num

#define hit(var, ...)                    HIT2_HIT_VARIADIC(var, HIT2_COUNTPARAM(__VA_ARGS__), __VA_ARGS__)
#define HIT2_HIT_VARIADIC(var, num, ...) hit_tileElemAtNoStride(var, num, __VA_ARGS__)

/* From hit_tile.h */
#define hit_tileElemAtNoStride(var, ndims, ...) hit_tileElemAtNoStride##ndims(var, __VA_ARGS__)

/* From hit_tileP.h */
#define hit_tileElemAtNoStride1(var, pos)                    ((var).data[pos])
#define hit_tileElemAtNoStride2(var, pos1, pos2)             ((var).data[(pos1) * (var).origAcumCard[1] + (pos2)])
#define hit_tileElemAtNoStride3(var, pos1, pos2, pos3)       ((var).data[(pos1) * (var).origAcumCard[1] + (pos2) * (var).origAcumCard[2] + (pos3)])
#define hit_tileElemAtNoStride4(var, pos1, pos2, pos3, pos4) ((var).data[(pos1) * (var).origAcumCard[1] + (pos2) * (var).origAcumCard[2] + (pos3) * (var).origAcumCard[3] + (pos4)])
