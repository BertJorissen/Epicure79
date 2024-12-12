#ifndef _CTRL_CORE_TEXDESC_H_
#define _CTRL_CORE_TEXDESC_H_
/**
 * @file Ctrl_TexDesc.h
 * @brief Structures to describe the configuration of textures.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

/**
 * Specifies the addressing mode for each dimension of the texture data.
 * Values are compatible with cudaTextureAddressMode.
 */
typedef enum Ctrl_TexAddrMode {
	CTRL_TEX_ADDRMODE_WRAP = 0,   /**< Not supported with non normalized coordinates.
								   * Wraps the texture coordinates around the texture boundary.
								   * For example, if the texture is 1D and has a width of 10,
								   * then a texture coordinate of -1.5 would be wrapped to 8.5,
								   * and a texture coordinate of 11.5 would be wrapped to 1.5.*/
	CTRL_TEX_ADDRMODE_CLAMP = 1,  /**< Clamps the texture coordinates to the edge of the texture.
								   * For example, if the texture is 1D and has a width of 10,
								   * then a texture coordinate of -1.5 would be clamped to 0,
								   * and a texture coordinate of 11.5 would be clamped to 9.*/
	CTRL_TEX_ADDRMODE_MIRROR = 2, /**< Not supported with non normalized coordinates.
								   * Mirrors the texture coordinates at every integer junction.
								   * For example, if the texture is 1D and has a width of 10,
								   * then a texture coordinate of -1.5 would be mirrored to 1.5,
								   * and a texture coordinate of 11.5 would be mirrored to 8.5.*/
	CTRL_TEX_ADDRMODE_BORDER = 3  /**< Returns a default value (usually 0) when the texture coordinates fall outside the texture.*/
} Ctrl_TexAddrMode;

/**
 * Specifies the interpolation mode to be used when fetching from the texture.
 * Values are compatible with cudaTextureFilterMode.
 *
 * @note On CUDA linear filtering is only supported if the texture return type is floating point.
 * This means either a texture containing floats or using normalized read mode.
 * @note On OpenCL linear filtering will always return floats, thus,
 *  a sampler created with linear filtering can only be used with read_imagef functions.
 */
typedef enum Ctrl_TexFilterMode {
	CTRL_TEX_FILTERMODE_POINT  = 0, /**< Returns the value of the nearest texel to the specified texture coordinates (nearest-neighbor).*/
	CTRL_TEX_FILTERMODE_LINEAR = 1  /**< Returns the weighted average of the four nearest texels to the specified texture coordinates (bilinear).*/
} Ctrl_TexFilterMode;

/**
 * Specifies whether integer data should be converted to floating point when reading the texture.
 * Values are compatible with cudaTextureReadMode.
 * This has no effect on OpenCL textures
 */
typedef enum Ctrl_TexReadMode {
	CTRL_TEX_READMODE_ELEMTYPE  = 0, /**< Read texture as specified element type.*/
	CTRL_TEX_READMODE_NORMFLOAT = 1  /**< Read texture as normalized float.*/
} Ctrl_TexReadMode;

/**
 * Configuration options for a texture.
 *
 * @note Currently only CUDA 2D pitched textures and OpenCL 2D images created from buffer are supported.
 */
typedef struct Ctrl_TexDesc {
	int                width;             /**< Width of the texture in elements. If 0 tile width will be used*/
	int                height;            /**< Height of the texture in elements. If 0 tile height will be used*/
	bool               normalized_coords; /**< Specifies whether the texture coordinates will be normalized or not*/
	Ctrl_TexAddrMode   addr_mode[3];      /**< Specifies the addressing mode for each dimension of the texture data. On OpenCL all dimensions must be the same, so only first value is checked.*/
	Ctrl_TexFilterMode filter_mode;       /**< Specifies the filtering mode to be used when fetching from the texture*/
	Ctrl_TexReadMode   read_mode;         /**< Specifies whether integer data should be converted to floating point or not*/
} Ctrl_TexDesc;

#endif // _CTRL_CORE_TEXDESC_H_