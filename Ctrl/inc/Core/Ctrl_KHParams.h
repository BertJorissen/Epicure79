#ifndef _CTRL_KHPARAMS_H_
#define _CTRL_KHPARAMS_H_
/**
 * @file Ctrl_KHParams.h
 * @brief Public macro-functions to convert lists of parameter declarations to
 * 	Kernel or Host-Task declaration syntax
 * 	Each parameter declaration includes three elements separated by commas:
 * 	role, type, name
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/* Macros to count parameters and groups of parameters */
#define CTRL_KHPARAMS_COUNT(...) CTRL_KHPARAMS_COUNT_N(__VA_ARGS__, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define CTRL_KHPARAMS_COUNT_N(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n49, n50, n51, n52, n53, n54, n55, n56, n57, num, ...) num

#define CTRL_KHPARAMS_COUNT_GROUPS(...) CTRL_KHPARAMS_COUNT_GROUPS_N(__VA_ARGS__, 10, 0, 0, 18, 0, 0, 17, 0, 0, 16, 0, 0, 15, 0, 0, 14, 0, 0, 13, 0, 0, 12, 0, 0, 11, 0, 0, 10, 0, 0, 9, 0, 0, 8, 0, 0, 7, 0, 0, 6, 0, 0, 5, 0, 0, 4, 0, 0, 3, 0, 0, 2, 0, 0, 1, 0, 0)

#define CTRL_KHPARAMS_COUNT_GROUPS_N(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31, n32, n33, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48, n49, n50, n51, n52, n53, n54, n55, n56, n57, num, ...) num

/* Transform Tiles declaration in KTiles declarations */
#define CTRL_KPARAMS_IN(t, n)    K##t n
#define CTRL_KPARAMS_OUT(t, n)   K##t n
#define CTRL_KPARAMS_IO(t, n)    K##t n
#define CTRL_KPARAMS_INVAL(t, n) t n

/**
 * Transform a string with parameter declarations into a kernel list of parameters
 *
 * @hideinitializer
 *
 * @param param_list list of parameter declarations: role, type, name [, ...]
 *
 * @see CTRL_KERNEL
 */
#define CTRL_KPARAMS_C(...)       CTRL_KPARAMS_C2(CTRL_KHPARAMS_COUNT_GROUPS(__VA_ARGS__), __VA_ARGS__)
#define CTRL_KPARAMS_C2(num, ...) CTRL_KPARAMS(num, __VA_ARGS__)

/**
 * Transform a string with parameter declarations into a host-task list of parameters
 *
 * @hideinitializer
 *
 * @param param_list list of parameter declarations: role, type, name [, ...]
 *
 * @see CTRL_KERNEL
 */
#define CTRL_HPARAMS_C(...)       CTRL_HPARAMS_C2(CTRL_KHPARAMS_COUNT_GROUPS(__VA_ARGS__), __VA_ARGS__)
#define CTRL_HPARAMS_C2(num, ...) CTRL_HPARAMS(num, __VA_ARGS__)

/**
 * Transform a string with parameter declarations preceded by the number of parameters into a kernel list of parameters
 *
 * @hideinitializer
 *
 * @param num number of parameter declarations
 * @param param_list list of parameter declarations: role, type, name [, ...]
 *
 * @see CTRL_KERNEL
 */
#define CTRL_KPARAMS(...)             CTRL_KPARAMS2(__VA_ARGS__)
#define CTRL_KPARAMS2(num, ...)       CTRL_KPARAMS_##num(__VA_ARGS__)
#define CTRL_KPARAMS_1(r, t, n)       CTRL_KPARAMS_##r(t, n)
#define CTRL_KPARAMS_2(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_1(__VA_ARGS__)
#define CTRL_KPARAMS_3(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_2(__VA_ARGS__)
#define CTRL_KPARAMS_4(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_3(__VA_ARGS__)
#define CTRL_KPARAMS_5(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_4(__VA_ARGS__)
#define CTRL_KPARAMS_6(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_5(__VA_ARGS__)
#define CTRL_KPARAMS_7(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_6(__VA_ARGS__)
#define CTRL_KPARAMS_8(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_7(__VA_ARGS__)
#define CTRL_KPARAMS_9(r, t, n, ...)  CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_8(__VA_ARGS__)
#define CTRL_KPARAMS_10(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_9(__VA_ARGS__)
#define CTRL_KPARAMS_11(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_10(__VA_ARGS__)
#define CTRL_KPARAMS_12(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_11(__VA_ARGS__)
#define CTRL_KPARAMS_13(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_12(__VA_ARGS__)
#define CTRL_KPARAMS_14(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_13(__VA_ARGS__)
#define CTRL_KPARAMS_15(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_14(__VA_ARGS__)
#define CTRL_KPARAMS_16(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_15(__VA_ARGS__)
#define CTRL_KPARAMS_17(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_16(__VA_ARGS__)
#define CTRL_KPARAMS_18(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_17(__VA_ARGS__)
#define CTRL_KPARAMS_19(r, t, n, ...) CTRL_KPARAMS_##r(t, n), CTRL_KPARAMS_18(__VA_ARGS__)

/**
 * Transform a string with parameter declarations preceded by the number of parameters into a host-task list of parameters
 *
 * @hideinitializer
 *
 * @param num number of parameter declarations
 * @param param_list list of parameter declarations: role, type, name [, ...]
 *
 * @see CTRL_KERNEL
 */
#define CTRL_HPARAMS(...)             CTRL_HPARAMS2(__VA_ARGS__)
#define CTRL_HPARAMS2(num, ...)       CTRL_HPARAMS_##num(__VA_ARGS__)
#define CTRL_HPARAMS_1(r, t, n)       t n
#define CTRL_HPARAMS_2(r, t, n, ...)  t n, CTRL_HPARAMS_1(__VA_ARGS__)
#define CTRL_HPARAMS_3(r, t, n, ...)  t n, CTRL_HPARAMS_2(__VA_ARGS__)
#define CTRL_HPARAMS_4(r, t, n, ...)  t n, CTRL_HPARAMS_3(__VA_ARGS__)
#define CTRL_HPARAMS_5(r, t, n, ...)  t n, CTRL_HPARAMS_4(__VA_ARGS__)
#define CTRL_HPARAMS_6(r, t, n, ...)  t n, CTRL_HPARAMS_5(__VA_ARGS__)
#define CTRL_HPARAMS_7(r, t, n, ...)  t n, CTRL_HPARAMS_6(__VA_ARGS__)
#define CTRL_HPARAMS_8(r, t, n, ...)  t n, CTRL_HPARAMS_7(__VA_ARGS__)
#define CTRL_HPARAMS_9(r, t, n, ...)  t n, CTRL_HPARAMS_8(__VA_ARGS__)
#define CTRL_HPARAMS_10(r, t, n, ...) t n, CTRL_HPARAMS_9(__VA_ARGS__)
#define CTRL_HPARAMS_11(r, t, n, ...) t n, CTRL_HPARAMS_10(__VA_ARGS__)
#define CTRL_HPARAMS_12(r, t, n, ...) t n, CTRL_HPARAMS_11(__VA_ARGS__)
#define CTRL_HPARAMS_13(r, t, n, ...) t n, CTRL_HPARAMS_12(__VA_ARGS__)
#define CTRL_HPARAMS_14(r, t, n, ...) t n, CTRL_HPARAMS_13(__VA_ARGS__)
#define CTRL_HPARAMS_15(r, t, n, ...) t n, CTRL_HPARAMS_14(__VA_ARGS__)
#define CTRL_HPARAMS_16(r, t, n, ...) t n, CTRL_HPARAMS_15(__VA_ARGS__)
#define CTRL_HPARAMS_17(r, t, n, ...) t n, CTRL_HPARAMS_16(__VA_ARGS__)
#define CTRL_HPARAMS_18(r, t, n, ...) t n, CTRL_HPARAMS_17(__VA_ARGS__)
#define CTRL_HPARAMS_19(r, t, n, ...) t n, CTRL_HPARAMS_18(__VA_ARGS__)

#endif
