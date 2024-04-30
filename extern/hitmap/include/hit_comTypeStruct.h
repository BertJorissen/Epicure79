/**
 * Macros to create the MPI Struct Datatypes.
 * The script to generate this file is included at the end.
 *
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
 *
 */

// Macro for 1 struct elements
#define hit_comTypeStruct1(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[1] = {(COUNT_A)}; \
	MPI_Datatype types[1] = {(TYPE_A)}; \
	MPI_Aint displs[1]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	int ivarloop; \
	for(ivarloop=0; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(1, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 2 struct elements
#define hit_comTypeStruct2(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[2] = {(COUNT_A), (COUNT_B)}; \
	MPI_Datatype types[2] = {(TYPE_A), (TYPE_B)}; \
	MPI_Aint displs[2]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	int ivarloop; \
	for(ivarloop=1; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(2, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 3 struct elements
#define hit_comTypeStruct3(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[3] = {(COUNT_A), (COUNT_B), (COUNT_C)}; \
	MPI_Datatype types[3] = {(TYPE_A), (TYPE_B), (TYPE_C)}; \
	MPI_Aint displs[3]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	int ivarloop; \
	for(ivarloop=2; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(3, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 4 struct elements
#define hit_comTypeStruct4(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[4] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D)}; \
	MPI_Datatype types[4] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D)}; \
	MPI_Aint displs[4]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	int ivarloop; \
	for(ivarloop=3; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(4, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 5 struct elements
#define hit_comTypeStruct5(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[5] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E)}; \
	MPI_Datatype types[5] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E)}; \
	MPI_Aint displs[5]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	int ivarloop; \
	for(ivarloop=4; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(5, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 6 struct elements
#define hit_comTypeStruct6(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[6] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F)}; \
	MPI_Datatype types[6] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F)}; \
	MPI_Aint displs[6]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	int ivarloop; \
	for(ivarloop=5; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(6, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 7 struct elements
#define hit_comTypeStruct7(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[7] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G)}; \
	MPI_Datatype types[7] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G)}; \
	MPI_Aint displs[7]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	int ivarloop; \
	for(ivarloop=6; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(7, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 8 struct elements
#define hit_comTypeStruct8(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[8] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H)}; \
	MPI_Datatype types[8] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H)}; \
	MPI_Aint displs[8]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	int ivarloop; \
	for(ivarloop=7; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(8, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 9 struct elements
#define hit_comTypeStruct9(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[9] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I)}; \
	MPI_Datatype types[9] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I)}; \
	MPI_Aint displs[9]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	int ivarloop; \
	for(ivarloop=8; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(9, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 10 struct elements
#define hit_comTypeStruct10(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[10] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J)}; \
	MPI_Datatype types[10] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J)}; \
	MPI_Aint displs[10]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	int ivarloop; \
	for(ivarloop=9; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(10, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 11 struct elements
#define hit_comTypeStruct11(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[11] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K)}; \
	MPI_Datatype types[11] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K)}; \
	MPI_Aint displs[11]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	int ivarloop; \
	for(ivarloop=10; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(11, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 12 struct elements
#define hit_comTypeStruct12(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[12] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L)}; \
	MPI_Datatype types[12] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L)}; \
	MPI_Aint displs[12]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	int ivarloop; \
	for(ivarloop=11; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(12, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 13 struct elements
#define hit_comTypeStruct13(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[13] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M)}; \
	MPI_Datatype types[13] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M)}; \
	MPI_Aint displs[13]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	int ivarloop; \
	for(ivarloop=12; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(13, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 14 struct elements
#define hit_comTypeStruct14(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[14] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N)}; \
	MPI_Datatype types[14] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N)}; \
	MPI_Aint displs[14]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	int ivarloop; \
	for(ivarloop=13; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(14, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 15 struct elements
#define hit_comTypeStruct15(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[15] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O)}; \
	MPI_Datatype types[15] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O)}; \
	MPI_Aint displs[15]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	int ivarloop; \
	for(ivarloop=14; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(15, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 16 struct elements
#define hit_comTypeStruct16(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[16] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P)}; \
	MPI_Datatype types[16] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P)}; \
	MPI_Aint displs[16]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	int ivarloop; \
	for(ivarloop=15; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(16, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 17 struct elements
#define hit_comTypeStruct17(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[17] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q)}; \
	MPI_Datatype types[17] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q)}; \
	MPI_Aint displs[17]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	int ivarloop; \
	for(ivarloop=16; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(17, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 18 struct elements
#define hit_comTypeStruct18(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[18] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R)}; \
	MPI_Datatype types[18] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R)}; \
	MPI_Aint displs[18]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	int ivarloop; \
	for(ivarloop=17; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(18, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 19 struct elements
#define hit_comTypeStruct19(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[19] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S)}; \
	MPI_Datatype types[19] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S)}; \
	MPI_Aint displs[19]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	int ivarloop; \
	for(ivarloop=18; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(19, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 20 struct elements
#define hit_comTypeStruct20(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[20] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T)}; \
	MPI_Datatype types[20] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T)}; \
	MPI_Aint displs[20]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	int ivarloop; \
	for(ivarloop=19; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(20, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 21 struct elements
#define hit_comTypeStruct21(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[21] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U)}; \
	MPI_Datatype types[21] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U)}; \
	MPI_Aint displs[21]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	int ivarloop; \
	for(ivarloop=20; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(21, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 22 struct elements
#define hit_comTypeStruct22(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[22] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V)}; \
	MPI_Datatype types[22] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V)}; \
	MPI_Aint displs[22]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	int ivarloop; \
	for(ivarloop=21; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(22, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 23 struct elements
#define hit_comTypeStruct23(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[23] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W)}; \
	MPI_Datatype types[23] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W)}; \
	MPI_Aint displs[23]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	int ivarloop; \
	for(ivarloop=22; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(23, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 24 struct elements
#define hit_comTypeStruct24(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[24] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X)}; \
	MPI_Datatype types[24] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X)}; \
	MPI_Aint displs[24]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	int ivarloop; \
	for(ivarloop=23; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(24, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 25 struct elements
#define hit_comTypeStruct25(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[25] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y)}; \
	MPI_Datatype types[25] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y)}; \
	MPI_Aint displs[25]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	int ivarloop; \
	for(ivarloop=24; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(25, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 26 struct elements
#define hit_comTypeStruct26(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[26] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z)}; \
	MPI_Datatype types[26] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z)}; \
	MPI_Aint displs[26]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	int ivarloop; \
	for(ivarloop=25; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(26, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 27 struct elements
#define hit_comTypeStruct27(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[27] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA)}; \
	MPI_Datatype types[27] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA)}; \
	MPI_Aint displs[27]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	int ivarloop; \
	for(ivarloop=26; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(27, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 28 struct elements
#define hit_comTypeStruct28(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[28] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB)}; \
	MPI_Datatype types[28] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB)}; \
	MPI_Aint displs[28]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	int ivarloop; \
	for(ivarloop=27; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(28, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 29 struct elements
#define hit_comTypeStruct29(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[29] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC)}; \
	MPI_Datatype types[29] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC)}; \
	MPI_Aint displs[29]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	int ivarloop; \
	for(ivarloop=28; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(29, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 30 struct elements
#define hit_comTypeStruct30(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[30] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD)}; \
	MPI_Datatype types[30] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD)}; \
	MPI_Aint displs[30]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	int ivarloop; \
	for(ivarloop=29; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(30, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 31 struct elements
#define hit_comTypeStruct31(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[31] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE)}; \
	MPI_Datatype types[31] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE)}; \
	MPI_Aint displs[31]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	int ivarloop; \
	for(ivarloop=30; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(31, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 32 struct elements
#define hit_comTypeStruct32(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[32] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF)}; \
	MPI_Datatype types[32] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF)}; \
	MPI_Aint displs[32]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	int ivarloop; \
	for(ivarloop=31; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(32, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 33 struct elements
#define hit_comTypeStruct33(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[33] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG)}; \
	MPI_Datatype types[33] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG)}; \
	MPI_Aint displs[33]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	int ivarloop; \
	for(ivarloop=32; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(33, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 34 struct elements
#define hit_comTypeStruct34(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[34] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH)}; \
	MPI_Datatype types[34] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH)}; \
	MPI_Aint displs[34]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	int ivarloop; \
	for(ivarloop=33; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(34, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 35 struct elements
#define hit_comTypeStruct35(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[35] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI)}; \
	MPI_Datatype types[35] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI)}; \
	MPI_Aint displs[35]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	int ivarloop; \
	for(ivarloop=34; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(35, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 36 struct elements
#define hit_comTypeStruct36(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[36] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ)}; \
	MPI_Datatype types[36] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ)}; \
	MPI_Aint displs[36]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	int ivarloop; \
	for(ivarloop=35; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(36, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 37 struct elements
#define hit_comTypeStruct37(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[37] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK)}; \
	MPI_Datatype types[37] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK)}; \
	MPI_Aint displs[37]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	int ivarloop; \
	for(ivarloop=36; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(37, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 38 struct elements
#define hit_comTypeStruct38(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[38] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL)}; \
	MPI_Datatype types[38] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL)}; \
	MPI_Aint displs[38]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	int ivarloop; \
	for(ivarloop=37; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(38, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 39 struct elements
#define hit_comTypeStruct39(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[39] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM)}; \
	MPI_Datatype types[39] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM)}; \
	MPI_Aint displs[39]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	int ivarloop; \
	for(ivarloop=38; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(39, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 40 struct elements
#define hit_comTypeStruct40(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[40] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN)}; \
	MPI_Datatype types[40] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN)}; \
	MPI_Aint displs[40]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	int ivarloop; \
	for(ivarloop=39; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(40, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 41 struct elements
#define hit_comTypeStruct41(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[41] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO)}; \
	MPI_Datatype types[41] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO)}; \
	MPI_Aint displs[41]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	int ivarloop; \
	for(ivarloop=40; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(41, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 42 struct elements
#define hit_comTypeStruct42(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[42] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP)}; \
	MPI_Datatype types[42] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP)}; \
	MPI_Aint displs[42]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	int ivarloop; \
	for(ivarloop=41; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(42, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 43 struct elements
#define hit_comTypeStruct43(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[43] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ)}; \
	MPI_Datatype types[43] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ)}; \
	MPI_Aint displs[43]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	int ivarloop; \
	for(ivarloop=42; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(43, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 44 struct elements
#define hit_comTypeStruct44(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[44] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR)}; \
	MPI_Datatype types[44] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR)}; \
	MPI_Aint displs[44]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	int ivarloop; \
	for(ivarloop=43; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(44, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 45 struct elements
#define hit_comTypeStruct45(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[45] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS)}; \
	MPI_Datatype types[45] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS)}; \
	MPI_Aint displs[45]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	int ivarloop; \
	for(ivarloop=44; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(45, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 46 struct elements
#define hit_comTypeStruct46(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[46] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT)}; \
	MPI_Datatype types[46] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT)}; \
	MPI_Aint displs[46]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	int ivarloop; \
	for(ivarloop=45; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(46, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 47 struct elements
#define hit_comTypeStruct47(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[47] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU)}; \
	MPI_Datatype types[47] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU)}; \
	MPI_Aint displs[47]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	int ivarloop; \
	for(ivarloop=46; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(47, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 48 struct elements
#define hit_comTypeStruct48(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[48] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV)}; \
	MPI_Datatype types[48] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV)}; \
	MPI_Aint displs[48]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	int ivarloop; \
	for(ivarloop=47; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(48, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 49 struct elements
#define hit_comTypeStruct49(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[49] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW)}; \
	MPI_Datatype types[49] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW)}; \
	MPI_Aint displs[49]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	int ivarloop; \
	for(ivarloop=48; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(49, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 50 struct elements
#define hit_comTypeStruct50(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[50] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX)}; \
	MPI_Datatype types[50] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX)}; \
	MPI_Aint displs[50]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	int ivarloop; \
	for(ivarloop=49; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(50, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 51 struct elements
#define hit_comTypeStruct51(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[51] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY)}; \
	MPI_Datatype types[51] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY)}; \
	MPI_Aint displs[51]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	int ivarloop; \
	for(ivarloop=50; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(51, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 52 struct elements
#define hit_comTypeStruct52(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[52] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ)}; \
	MPI_Datatype types[52] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ)}; \
	MPI_Aint displs[52]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	int ivarloop; \
	for(ivarloop=51; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(52, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 53 struct elements
#define hit_comTypeStruct53(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[53] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA)}; \
	MPI_Datatype types[53] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA)}; \
	MPI_Aint displs[53]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	int ivarloop; \
	for(ivarloop=52; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(53, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 54 struct elements
#define hit_comTypeStruct54(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[54] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB)}; \
	MPI_Datatype types[54] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB)}; \
	MPI_Aint displs[54]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	int ivarloop; \
	for(ivarloop=53; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(54, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 55 struct elements
#define hit_comTypeStruct55(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[55] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC)}; \
	MPI_Datatype types[55] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC)}; \
	MPI_Aint displs[55]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	int ivarloop; \
	for(ivarloop=54; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(55, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 56 struct elements
#define hit_comTypeStruct56(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[56] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD)}; \
	MPI_Datatype types[56] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD)}; \
	MPI_Aint displs[56]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	int ivarloop; \
	for(ivarloop=55; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(56, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 57 struct elements
#define hit_comTypeStruct57(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[57] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE)}; \
	MPI_Datatype types[57] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE)}; \
	MPI_Aint displs[57]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	int ivarloop; \
	for(ivarloop=56; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(57, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 58 struct elements
#define hit_comTypeStruct58(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[58] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF)}; \
	MPI_Datatype types[58] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF)}; \
	MPI_Aint displs[58]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	int ivarloop; \
	for(ivarloop=57; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(58, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 59 struct elements
#define hit_comTypeStruct59(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[59] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG)}; \
	MPI_Datatype types[59] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG)}; \
	MPI_Aint displs[59]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	int ivarloop; \
	for(ivarloop=58; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(59, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 60 struct elements
#define hit_comTypeStruct60(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[60] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH)}; \
	MPI_Datatype types[60] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH)}; \
	MPI_Aint displs[60]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	int ivarloop; \
	for(ivarloop=59; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(60, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 61 struct elements
#define hit_comTypeStruct61(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[61] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI)}; \
	MPI_Datatype types[61] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI)}; \
	MPI_Aint displs[61]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	int ivarloop; \
	for(ivarloop=60; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(61, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 62 struct elements
#define hit_comTypeStruct62(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[62] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ)}; \
	MPI_Datatype types[62] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ)}; \
	MPI_Aint displs[62]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	int ivarloop; \
	for(ivarloop=61; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(62, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 63 struct elements
#define hit_comTypeStruct63(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[63] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK)}; \
	MPI_Datatype types[63] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK)}; \
	MPI_Aint displs[63]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	int ivarloop; \
	for(ivarloop=62; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(63, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 64 struct elements
#define hit_comTypeStruct64(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[64] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL)}; \
	MPI_Datatype types[64] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL)}; \
	MPI_Aint displs[64]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	int ivarloop; \
	for(ivarloop=63; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(64, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 65 struct elements
#define hit_comTypeStruct65(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[65] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM)}; \
	MPI_Datatype types[65] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM)}; \
	MPI_Aint displs[65]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	int ivarloop; \
	for(ivarloop=64; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(65, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 66 struct elements
#define hit_comTypeStruct66(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[66] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN)}; \
	MPI_Datatype types[66] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN)}; \
	MPI_Aint displs[66]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	int ivarloop; \
	for(ivarloop=65; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(66, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 67 struct elements
#define hit_comTypeStruct67(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[67] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO)}; \
	MPI_Datatype types[67] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO)}; \
	MPI_Aint displs[67]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	int ivarloop; \
	for(ivarloop=66; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(67, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 68 struct elements
#define hit_comTypeStruct68(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[68] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP)}; \
	MPI_Datatype types[68] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP)}; \
	MPI_Aint displs[68]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	int ivarloop; \
	for(ivarloop=67; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(68, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 69 struct elements
#define hit_comTypeStruct69(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[69] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ)}; \
	MPI_Datatype types[69] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ)}; \
	MPI_Aint displs[69]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	int ivarloop; \
	for(ivarloop=68; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(69, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 70 struct elements
#define hit_comTypeStruct70(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[70] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR)}; \
	MPI_Datatype types[70] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR)}; \
	MPI_Aint displs[70]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	int ivarloop; \
	for(ivarloop=69; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(70, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 71 struct elements
#define hit_comTypeStruct71(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[71] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS)}; \
	MPI_Datatype types[71] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS)}; \
	MPI_Aint displs[71]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	int ivarloop; \
	for(ivarloop=70; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(71, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 72 struct elements
#define hit_comTypeStruct72(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[72] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT)}; \
	MPI_Datatype types[72] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT)}; \
	MPI_Aint displs[72]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	int ivarloop; \
	for(ivarloop=71; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(72, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 73 struct elements
#define hit_comTypeStruct73(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[73] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU)}; \
	MPI_Datatype types[73] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU)}; \
	MPI_Aint displs[73]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	int ivarloop; \
	for(ivarloop=72; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(73, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 74 struct elements
#define hit_comTypeStruct74(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[74] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV)}; \
	MPI_Datatype types[74] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV)}; \
	MPI_Aint displs[74]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	int ivarloop; \
	for(ivarloop=73; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(74, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 75 struct elements
#define hit_comTypeStruct75(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[75] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW)}; \
	MPI_Datatype types[75] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW)}; \
	MPI_Aint displs[75]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	int ivarloop; \
	for(ivarloop=74; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(75, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 76 struct elements
#define hit_comTypeStruct76(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[76] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX)}; \
	MPI_Datatype types[76] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX)}; \
	MPI_Aint displs[76]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	int ivarloop; \
	for(ivarloop=75; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(76, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 77 struct elements
#define hit_comTypeStruct77(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[77] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY)}; \
	MPI_Datatype types[77] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY)}; \
	MPI_Aint displs[77]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	int ivarloop; \
	for(ivarloop=76; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(77, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 78 struct elements
#define hit_comTypeStruct78(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[78] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ)}; \
	MPI_Datatype types[78] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ)}; \
	MPI_Aint displs[78]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	int ivarloop; \
	for(ivarloop=77; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(78, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 79 struct elements
#define hit_comTypeStruct79(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[79] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA)}; \
	MPI_Datatype types[79] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA)}; \
	MPI_Aint displs[79]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	int ivarloop; \
	for(ivarloop=78; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(79, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 80 struct elements
#define hit_comTypeStruct80(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[80] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB)}; \
	MPI_Datatype types[80] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB)}; \
	MPI_Aint displs[80]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	int ivarloop; \
	for(ivarloop=79; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(80, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 81 struct elements
#define hit_comTypeStruct81(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[81] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC)}; \
	MPI_Datatype types[81] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC)}; \
	MPI_Aint displs[81]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	int ivarloop; \
	for(ivarloop=80; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(81, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 82 struct elements
#define hit_comTypeStruct82(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[82] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD)}; \
	MPI_Datatype types[82] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD)}; \
	MPI_Aint displs[82]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	int ivarloop; \
	for(ivarloop=81; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(82, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 83 struct elements
#define hit_comTypeStruct83(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[83] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE)}; \
	MPI_Datatype types[83] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE)}; \
	MPI_Aint displs[83]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	int ivarloop; \
	for(ivarloop=82; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(83, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 84 struct elements
#define hit_comTypeStruct84(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[84] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF)}; \
	MPI_Datatype types[84] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF)}; \
	MPI_Aint displs[84]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	int ivarloop; \
	for(ivarloop=83; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(84, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 85 struct elements
#define hit_comTypeStruct85(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[85] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG)}; \
	MPI_Datatype types[85] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG)}; \
	MPI_Aint displs[85]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	int ivarloop; \
	for(ivarloop=84; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(85, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 86 struct elements
#define hit_comTypeStruct86(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[86] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH)}; \
	MPI_Datatype types[86] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH)}; \
	MPI_Aint displs[86]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	int ivarloop; \
	for(ivarloop=85; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(86, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 87 struct elements
#define hit_comTypeStruct87(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[87] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI)}; \
	MPI_Datatype types[87] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI)}; \
	MPI_Aint displs[87]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	int ivarloop; \
	for(ivarloop=86; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(87, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 88 struct elements
#define hit_comTypeStruct88(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[88] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ)}; \
	MPI_Datatype types[88] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ)}; \
	MPI_Aint displs[88]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	int ivarloop; \
	for(ivarloop=87; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(88, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 89 struct elements
#define hit_comTypeStruct89(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[89] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK)}; \
	MPI_Datatype types[89] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK)}; \
	MPI_Aint displs[89]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	int ivarloop; \
	for(ivarloop=88; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(89, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 90 struct elements
#define hit_comTypeStruct90(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[90] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL)}; \
	MPI_Datatype types[90] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL)}; \
	MPI_Aint displs[90]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	int ivarloop; \
	for(ivarloop=89; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(90, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 91 struct elements
#define hit_comTypeStruct91(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[91] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM)}; \
	MPI_Datatype types[91] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM)}; \
	MPI_Aint displs[91]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	int ivarloop; \
	for(ivarloop=90; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(91, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 92 struct elements
#define hit_comTypeStruct92(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[92] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN)}; \
	MPI_Datatype types[92] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN)}; \
	MPI_Aint displs[92]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	int ivarloop; \
	for(ivarloop=91; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(92, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 93 struct elements
#define hit_comTypeStruct93(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[93] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO)}; \
	MPI_Datatype types[93] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO)}; \
	MPI_Aint displs[93]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	int ivarloop; \
	for(ivarloop=92; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(93, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 94 struct elements
#define hit_comTypeStruct94(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[94] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP)}; \
	MPI_Datatype types[94] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP)}; \
	MPI_Aint displs[94]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	int ivarloop; \
	for(ivarloop=93; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(94, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 95 struct elements
#define hit_comTypeStruct95(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[95] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ)}; \
	MPI_Datatype types[95] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ)}; \
	MPI_Aint displs[95]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	int ivarloop; \
	for(ivarloop=94; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(95, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 96 struct elements
#define hit_comTypeStruct96(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ, NAME_CR, COUNT_CR, TYPE_CR) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[96] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ), (COUNT_CR)}; \
	MPI_Datatype types[96] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ), (TYPE_CR)}; \
	MPI_Aint displs[96]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	MPI_Get_address(&cmdline.NAME_CR, &displs[95]); \
	int ivarloop; \
	for(ivarloop=95; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(96, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 97 struct elements
#define hit_comTypeStruct97(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ, NAME_CR, COUNT_CR, TYPE_CR, NAME_CS, COUNT_CS, TYPE_CS) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[97] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ), (COUNT_CR), (COUNT_CS)}; \
	MPI_Datatype types[97] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ), (TYPE_CR), (TYPE_CS)}; \
	MPI_Aint displs[97]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	MPI_Get_address(&cmdline.NAME_CR, &displs[95]); \
	MPI_Get_address(&cmdline.NAME_CS, &displs[96]); \
	int ivarloop; \
	for(ivarloop=96; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(97, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 98 struct elements
#define hit_comTypeStruct98(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ, NAME_CR, COUNT_CR, TYPE_CR, NAME_CS, COUNT_CS, TYPE_CS, NAME_CT, COUNT_CT, TYPE_CT) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[98] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ), (COUNT_CR), (COUNT_CS), (COUNT_CT)}; \
	MPI_Datatype types[98] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ), (TYPE_CR), (TYPE_CS), (TYPE_CT)}; \
	MPI_Aint displs[98]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	MPI_Get_address(&cmdline.NAME_CR, &displs[95]); \
	MPI_Get_address(&cmdline.NAME_CS, &displs[96]); \
	MPI_Get_address(&cmdline.NAME_CT, &displs[97]); \
	int ivarloop; \
	for(ivarloop=97; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(98, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 99 struct elements
#define hit_comTypeStruct99(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ, NAME_CR, COUNT_CR, TYPE_CR, NAME_CS, COUNT_CS, TYPE_CS, NAME_CT, COUNT_CT, TYPE_CT, NAME_CU, COUNT_CU, TYPE_CU) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[99] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ), (COUNT_CR), (COUNT_CS), (COUNT_CT), (COUNT_CU)}; \
	MPI_Datatype types[99] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ), (TYPE_CR), (TYPE_CS), (TYPE_CT), (TYPE_CU)}; \
	MPI_Aint displs[99]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	MPI_Get_address(&cmdline.NAME_CR, &displs[95]); \
	MPI_Get_address(&cmdline.NAME_CS, &displs[96]); \
	MPI_Get_address(&cmdline.NAME_CT, &displs[97]); \
	MPI_Get_address(&cmdline.NAME_CU, &displs[98]); \
	int ivarloop; \
	for(ivarloop=98; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(99, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}

// Macro for 100 struct elements
#define hit_comTypeStruct100(new_type, NATIVE_STRUCT, NAME_A, COUNT_A, TYPE_A, NAME_B, COUNT_B, TYPE_B, NAME_C, COUNT_C, TYPE_C, NAME_D, COUNT_D, TYPE_D, NAME_E, COUNT_E, TYPE_E, NAME_F, COUNT_F, TYPE_F, NAME_G, COUNT_G, TYPE_G, NAME_H, COUNT_H, TYPE_H, NAME_I, COUNT_I, TYPE_I, NAME_J, COUNT_J, TYPE_J, NAME_K, COUNT_K, TYPE_K, NAME_L, COUNT_L, TYPE_L, NAME_M, COUNT_M, TYPE_M, NAME_N, COUNT_N, TYPE_N, NAME_O, COUNT_O, TYPE_O, NAME_P, COUNT_P, TYPE_P, NAME_Q, COUNT_Q, TYPE_Q, NAME_R, COUNT_R, TYPE_R, NAME_S, COUNT_S, TYPE_S, NAME_T, COUNT_T, TYPE_T, NAME_U, COUNT_U, TYPE_U, NAME_V, COUNT_V, TYPE_V, NAME_W, COUNT_W, TYPE_W, NAME_X, COUNT_X, TYPE_X, NAME_Y, COUNT_Y, TYPE_Y, NAME_Z, COUNT_Z, TYPE_Z, NAME_AA, COUNT_AA, TYPE_AA, NAME_AB, COUNT_AB, TYPE_AB, NAME_AC, COUNT_AC, TYPE_AC, NAME_AD, COUNT_AD, TYPE_AD, NAME_AE, COUNT_AE, TYPE_AE, NAME_AF, COUNT_AF, TYPE_AF, NAME_AG, COUNT_AG, TYPE_AG, NAME_AH, COUNT_AH, TYPE_AH, NAME_AI, COUNT_AI, TYPE_AI, NAME_AJ, COUNT_AJ, TYPE_AJ, NAME_AK, COUNT_AK, TYPE_AK, NAME_AL, COUNT_AL, TYPE_AL, NAME_AM, COUNT_AM, TYPE_AM, NAME_AN, COUNT_AN, TYPE_AN, NAME_AO, COUNT_AO, TYPE_AO, NAME_AP, COUNT_AP, TYPE_AP, NAME_AQ, COUNT_AQ, TYPE_AQ, NAME_AR, COUNT_AR, TYPE_AR, NAME_AS, COUNT_AS, TYPE_AS, NAME_AT, COUNT_AT, TYPE_AT, NAME_AU, COUNT_AU, TYPE_AU, NAME_AV, COUNT_AV, TYPE_AV, NAME_AW, COUNT_AW, TYPE_AW, NAME_AX, COUNT_AX, TYPE_AX, NAME_AY, COUNT_AY, TYPE_AY, NAME_AZ, COUNT_AZ, TYPE_AZ, NAME_BA, COUNT_BA, TYPE_BA, NAME_BB, COUNT_BB, TYPE_BB, NAME_BC, COUNT_BC, TYPE_BC, NAME_BD, COUNT_BD, TYPE_BD, NAME_BE, COUNT_BE, TYPE_BE, NAME_BF, COUNT_BF, TYPE_BF, NAME_BG, COUNT_BG, TYPE_BG, NAME_BH, COUNT_BH, TYPE_BH, NAME_BI, COUNT_BI, TYPE_BI, NAME_BJ, COUNT_BJ, TYPE_BJ, NAME_BK, COUNT_BK, TYPE_BK, NAME_BL, COUNT_BL, TYPE_BL, NAME_BM, COUNT_BM, TYPE_BM, NAME_BN, COUNT_BN, TYPE_BN, NAME_BO, COUNT_BO, TYPE_BO, NAME_BP, COUNT_BP, TYPE_BP, NAME_BQ, COUNT_BQ, TYPE_BQ, NAME_BR, COUNT_BR, TYPE_BR, NAME_BS, COUNT_BS, TYPE_BS, NAME_BT, COUNT_BT, TYPE_BT, NAME_BU, COUNT_BU, TYPE_BU, NAME_BV, COUNT_BV, TYPE_BV, NAME_BW, COUNT_BW, TYPE_BW, NAME_BX, COUNT_BX, TYPE_BX, NAME_BY, COUNT_BY, TYPE_BY, NAME_BZ, COUNT_BZ, TYPE_BZ, NAME_CA, COUNT_CA, TYPE_CA, NAME_CB, COUNT_CB, TYPE_CB, NAME_CC, COUNT_CC, TYPE_CC, NAME_CD, COUNT_CD, TYPE_CD, NAME_CE, COUNT_CE, TYPE_CE, NAME_CF, COUNT_CF, TYPE_CF, NAME_CG, COUNT_CG, TYPE_CG, NAME_CH, COUNT_CH, TYPE_CH, NAME_CI, COUNT_CI, TYPE_CI, NAME_CJ, COUNT_CJ, TYPE_CJ, NAME_CK, COUNT_CK, TYPE_CK, NAME_CL, COUNT_CL, TYPE_CL, NAME_CM, COUNT_CM, TYPE_CM, NAME_CN, COUNT_CN, TYPE_CN, NAME_CO, COUNT_CO, TYPE_CO, NAME_CP, COUNT_CP, TYPE_CP, NAME_CQ, COUNT_CQ, TYPE_CQ, NAME_CR, COUNT_CR, TYPE_CR, NAME_CS, COUNT_CS, TYPE_CS, NAME_CT, COUNT_CT, TYPE_CT, NAME_CU, COUNT_CU, TYPE_CU, NAME_CV, COUNT_CV, TYPE_CV) \
{ \
	NATIVE_STRUCT cmdline; \
	int blockcounts[100] = {(COUNT_A), (COUNT_B), (COUNT_C), (COUNT_D), (COUNT_E), (COUNT_F), (COUNT_G), (COUNT_H), (COUNT_I), (COUNT_J), (COUNT_K), (COUNT_L), (COUNT_M), (COUNT_N), (COUNT_O), (COUNT_P), (COUNT_Q), (COUNT_R), (COUNT_S), (COUNT_T), (COUNT_U), (COUNT_V), (COUNT_W), (COUNT_X), (COUNT_Y), (COUNT_Z), (COUNT_AA), (COUNT_AB), (COUNT_AC), (COUNT_AD), (COUNT_AE), (COUNT_AF), (COUNT_AG), (COUNT_AH), (COUNT_AI), (COUNT_AJ), (COUNT_AK), (COUNT_AL), (COUNT_AM), (COUNT_AN), (COUNT_AO), (COUNT_AP), (COUNT_AQ), (COUNT_AR), (COUNT_AS), (COUNT_AT), (COUNT_AU), (COUNT_AV), (COUNT_AW), (COUNT_AX), (COUNT_AY), (COUNT_AZ), (COUNT_BA), (COUNT_BB), (COUNT_BC), (COUNT_BD), (COUNT_BE), (COUNT_BF), (COUNT_BG), (COUNT_BH), (COUNT_BI), (COUNT_BJ), (COUNT_BK), (COUNT_BL), (COUNT_BM), (COUNT_BN), (COUNT_BO), (COUNT_BP), (COUNT_BQ), (COUNT_BR), (COUNT_BS), (COUNT_BT), (COUNT_BU), (COUNT_BV), (COUNT_BW), (COUNT_BX), (COUNT_BY), (COUNT_BZ), (COUNT_CA), (COUNT_CB), (COUNT_CC), (COUNT_CD), (COUNT_CE), (COUNT_CF), (COUNT_CG), (COUNT_CH), (COUNT_CI), (COUNT_CJ), (COUNT_CK), (COUNT_CL), (COUNT_CM), (COUNT_CN), (COUNT_CO), (COUNT_CP), (COUNT_CQ), (COUNT_CR), (COUNT_CS), (COUNT_CT), (COUNT_CU), (COUNT_CV)}; \
	MPI_Datatype types[100] = {(TYPE_A), (TYPE_B), (TYPE_C), (TYPE_D), (TYPE_E), (TYPE_F), (TYPE_G), (TYPE_H), (TYPE_I), (TYPE_J), (TYPE_K), (TYPE_L), (TYPE_M), (TYPE_N), (TYPE_O), (TYPE_P), (TYPE_Q), (TYPE_R), (TYPE_S), (TYPE_T), (TYPE_U), (TYPE_V), (TYPE_W), (TYPE_X), (TYPE_Y), (TYPE_Z), (TYPE_AA), (TYPE_AB), (TYPE_AC), (TYPE_AD), (TYPE_AE), (TYPE_AF), (TYPE_AG), (TYPE_AH), (TYPE_AI), (TYPE_AJ), (TYPE_AK), (TYPE_AL), (TYPE_AM), (TYPE_AN), (TYPE_AO), (TYPE_AP), (TYPE_AQ), (TYPE_AR), (TYPE_AS), (TYPE_AT), (TYPE_AU), (TYPE_AV), (TYPE_AW), (TYPE_AX), (TYPE_AY), (TYPE_AZ), (TYPE_BA), (TYPE_BB), (TYPE_BC), (TYPE_BD), (TYPE_BE), (TYPE_BF), (TYPE_BG), (TYPE_BH), (TYPE_BI), (TYPE_BJ), (TYPE_BK), (TYPE_BL), (TYPE_BM), (TYPE_BN), (TYPE_BO), (TYPE_BP), (TYPE_BQ), (TYPE_BR), (TYPE_BS), (TYPE_BT), (TYPE_BU), (TYPE_BV), (TYPE_BW), (TYPE_BX), (TYPE_BY), (TYPE_BZ), (TYPE_CA), (TYPE_CB), (TYPE_CC), (TYPE_CD), (TYPE_CE), (TYPE_CF), (TYPE_CG), (TYPE_CH), (TYPE_CI), (TYPE_CJ), (TYPE_CK), (TYPE_CL), (TYPE_CM), (TYPE_CN), (TYPE_CO), (TYPE_CP), (TYPE_CQ), (TYPE_CR), (TYPE_CS), (TYPE_CT), (TYPE_CU), (TYPE_CV)}; \
	MPI_Aint displs[100]; \
	MPI_Get_address(&cmdline.NAME_A, &displs[0]); \
	MPI_Get_address(&cmdline.NAME_B, &displs[1]); \
	MPI_Get_address(&cmdline.NAME_C, &displs[2]); \
	MPI_Get_address(&cmdline.NAME_D, &displs[3]); \
	MPI_Get_address(&cmdline.NAME_E, &displs[4]); \
	MPI_Get_address(&cmdline.NAME_F, &displs[5]); \
	MPI_Get_address(&cmdline.NAME_G, &displs[6]); \
	MPI_Get_address(&cmdline.NAME_H, &displs[7]); \
	MPI_Get_address(&cmdline.NAME_I, &displs[8]); \
	MPI_Get_address(&cmdline.NAME_J, &displs[9]); \
	MPI_Get_address(&cmdline.NAME_K, &displs[10]); \
	MPI_Get_address(&cmdline.NAME_L, &displs[11]); \
	MPI_Get_address(&cmdline.NAME_M, &displs[12]); \
	MPI_Get_address(&cmdline.NAME_N, &displs[13]); \
	MPI_Get_address(&cmdline.NAME_O, &displs[14]); \
	MPI_Get_address(&cmdline.NAME_P, &displs[15]); \
	MPI_Get_address(&cmdline.NAME_Q, &displs[16]); \
	MPI_Get_address(&cmdline.NAME_R, &displs[17]); \
	MPI_Get_address(&cmdline.NAME_S, &displs[18]); \
	MPI_Get_address(&cmdline.NAME_T, &displs[19]); \
	MPI_Get_address(&cmdline.NAME_U, &displs[20]); \
	MPI_Get_address(&cmdline.NAME_V, &displs[21]); \
	MPI_Get_address(&cmdline.NAME_W, &displs[22]); \
	MPI_Get_address(&cmdline.NAME_X, &displs[23]); \
	MPI_Get_address(&cmdline.NAME_Y, &displs[24]); \
	MPI_Get_address(&cmdline.NAME_Z, &displs[25]); \
	MPI_Get_address(&cmdline.NAME_AA, &displs[26]); \
	MPI_Get_address(&cmdline.NAME_AB, &displs[27]); \
	MPI_Get_address(&cmdline.NAME_AC, &displs[28]); \
	MPI_Get_address(&cmdline.NAME_AD, &displs[29]); \
	MPI_Get_address(&cmdline.NAME_AE, &displs[30]); \
	MPI_Get_address(&cmdline.NAME_AF, &displs[31]); \
	MPI_Get_address(&cmdline.NAME_AG, &displs[32]); \
	MPI_Get_address(&cmdline.NAME_AH, &displs[33]); \
	MPI_Get_address(&cmdline.NAME_AI, &displs[34]); \
	MPI_Get_address(&cmdline.NAME_AJ, &displs[35]); \
	MPI_Get_address(&cmdline.NAME_AK, &displs[36]); \
	MPI_Get_address(&cmdline.NAME_AL, &displs[37]); \
	MPI_Get_address(&cmdline.NAME_AM, &displs[38]); \
	MPI_Get_address(&cmdline.NAME_AN, &displs[39]); \
	MPI_Get_address(&cmdline.NAME_AO, &displs[40]); \
	MPI_Get_address(&cmdline.NAME_AP, &displs[41]); \
	MPI_Get_address(&cmdline.NAME_AQ, &displs[42]); \
	MPI_Get_address(&cmdline.NAME_AR, &displs[43]); \
	MPI_Get_address(&cmdline.NAME_AS, &displs[44]); \
	MPI_Get_address(&cmdline.NAME_AT, &displs[45]); \
	MPI_Get_address(&cmdline.NAME_AU, &displs[46]); \
	MPI_Get_address(&cmdline.NAME_AV, &displs[47]); \
	MPI_Get_address(&cmdline.NAME_AW, &displs[48]); \
	MPI_Get_address(&cmdline.NAME_AX, &displs[49]); \
	MPI_Get_address(&cmdline.NAME_AY, &displs[50]); \
	MPI_Get_address(&cmdline.NAME_AZ, &displs[51]); \
	MPI_Get_address(&cmdline.NAME_BA, &displs[52]); \
	MPI_Get_address(&cmdline.NAME_BB, &displs[53]); \
	MPI_Get_address(&cmdline.NAME_BC, &displs[54]); \
	MPI_Get_address(&cmdline.NAME_BD, &displs[55]); \
	MPI_Get_address(&cmdline.NAME_BE, &displs[56]); \
	MPI_Get_address(&cmdline.NAME_BF, &displs[57]); \
	MPI_Get_address(&cmdline.NAME_BG, &displs[58]); \
	MPI_Get_address(&cmdline.NAME_BH, &displs[59]); \
	MPI_Get_address(&cmdline.NAME_BI, &displs[60]); \
	MPI_Get_address(&cmdline.NAME_BJ, &displs[61]); \
	MPI_Get_address(&cmdline.NAME_BK, &displs[62]); \
	MPI_Get_address(&cmdline.NAME_BL, &displs[63]); \
	MPI_Get_address(&cmdline.NAME_BM, &displs[64]); \
	MPI_Get_address(&cmdline.NAME_BN, &displs[65]); \
	MPI_Get_address(&cmdline.NAME_BO, &displs[66]); \
	MPI_Get_address(&cmdline.NAME_BP, &displs[67]); \
	MPI_Get_address(&cmdline.NAME_BQ, &displs[68]); \
	MPI_Get_address(&cmdline.NAME_BR, &displs[69]); \
	MPI_Get_address(&cmdline.NAME_BS, &displs[70]); \
	MPI_Get_address(&cmdline.NAME_BT, &displs[71]); \
	MPI_Get_address(&cmdline.NAME_BU, &displs[72]); \
	MPI_Get_address(&cmdline.NAME_BV, &displs[73]); \
	MPI_Get_address(&cmdline.NAME_BW, &displs[74]); \
	MPI_Get_address(&cmdline.NAME_BX, &displs[75]); \
	MPI_Get_address(&cmdline.NAME_BY, &displs[76]); \
	MPI_Get_address(&cmdline.NAME_BZ, &displs[77]); \
	MPI_Get_address(&cmdline.NAME_CA, &displs[78]); \
	MPI_Get_address(&cmdline.NAME_CB, &displs[79]); \
	MPI_Get_address(&cmdline.NAME_CC, &displs[80]); \
	MPI_Get_address(&cmdline.NAME_CD, &displs[81]); \
	MPI_Get_address(&cmdline.NAME_CE, &displs[82]); \
	MPI_Get_address(&cmdline.NAME_CF, &displs[83]); \
	MPI_Get_address(&cmdline.NAME_CG, &displs[84]); \
	MPI_Get_address(&cmdline.NAME_CH, &displs[85]); \
	MPI_Get_address(&cmdline.NAME_CI, &displs[86]); \
	MPI_Get_address(&cmdline.NAME_CJ, &displs[87]); \
	MPI_Get_address(&cmdline.NAME_CK, &displs[88]); \
	MPI_Get_address(&cmdline.NAME_CL, &displs[89]); \
	MPI_Get_address(&cmdline.NAME_CM, &displs[90]); \
	MPI_Get_address(&cmdline.NAME_CN, &displs[91]); \
	MPI_Get_address(&cmdline.NAME_CO, &displs[92]); \
	MPI_Get_address(&cmdline.NAME_CP, &displs[93]); \
	MPI_Get_address(&cmdline.NAME_CQ, &displs[94]); \
	MPI_Get_address(&cmdline.NAME_CR, &displs[95]); \
	MPI_Get_address(&cmdline.NAME_CS, &displs[96]); \
	MPI_Get_address(&cmdline.NAME_CT, &displs[97]); \
	MPI_Get_address(&cmdline.NAME_CU, &displs[98]); \
	MPI_Get_address(&cmdline.NAME_CV, &displs[99]); \
	int ivarloop; \
	for(ivarloop=99; ivarloop>=0; ivarloop--){ \
		displs[ivarloop] -= displs[0]; \
	} \
	MPI_Type_create_struct(100, blockcounts, displs, types, new_type); \
	MPI_Type_commit(new_type); \
}
