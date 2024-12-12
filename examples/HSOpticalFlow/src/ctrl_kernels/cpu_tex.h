/**
 * @file solverKernel.h
 * @brief Texture operations for CPU Controller devices, used for HSOpticalflow.
 *
 * @copyright This software is part of the Controller project by Trasgo Group, UVa.
 * The relevant license, warranty and copyright notice is available in the Controller project repository.
 */

#include <math.h>

///////////////////////////////////////////////////////////////////////////////
/// \brief host texture fetch
///
/// read from arbitrary position within image using bilinear interpolation
/// out of range coords are mirrored
/// \param[in]  t   texture raw data
/// \param[in]  w   texture width
/// \param[in]  h   texture height
/// \param[in]  s   texture stride
/// \param[in]  x   x coord of the point to fetch value at
/// \param[in]  y   y coord of the point to fetch value at
/// \return fetched value
///////////////////////////////////////////////////////////////////////////////
static inline float cpu_Tex2D(KHitTile_float t, int w, int h, float x, float y) {
	// integer parts in floating point format
	float intPartX, intPartY;

	// get fractional parts of coordinates
	float dx = fabsf(modff(x, &intPartX));
	float dy = fabsf(modff(y, &intPartY));

	// assume pixels are squares
	// one of the corners
	int ix0 = (int)intPartX;
	int iy0 = (int)intPartY;

	// mirror out-of-range position
	if (ix0 < 0) ix0 = abs(ix0 + 1);

	if (iy0 < 0) iy0 = abs(iy0 + 1);

	if (ix0 >= w) ix0 = w * 2 - ix0 - 1;

	if (iy0 >= h) iy0 = h * 2 - iy0 - 1;

	// corner which is opposite to (ix0, iy0)
	int ix1 = ix0 + 1;
	int iy1 = iy0 + 1;

	if (ix1 >= w) ix1 = w * 2 - ix1 - 1;

	if (iy1 >= h) iy1 = h * 2 - iy1 - 1;

	float res = hit(t, iy0, ix0) * (1.0f - dx) * (1.0f - dy);
	res += hit(t, iy0, ix1) * dx * (1.0f - dy);
	res += hit(t, iy1, ix0) * (1.0f - dx) * dy;
	res += hit(t, iy1, ix1) * dx * dy;

	return res;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief host texture fetch
///
/// read specific texel value
/// out of range coords are mirrored
/// \param[in]  t   texture raw data
/// \param[in]  w   texture width
/// \param[in]  h   texture height
/// \param[in]  s   texture stride
/// \param[in]  x   x coord of the point to fetch value at
/// \param[in]  y   y coord of the point to fetch value at
/// \return fetched value
///////////////////////////////////////////////////////////////////////////////
static inline float cpu_Tex2Di(KHitTile_float src, int w, int h, int x, int y) {
	if (x < 0) x = abs(x + 1);

	if (y < 0) y = abs(y + 1);

	if (x >= w) x = w * 2 - x - 1;

	if (y >= h) y = h * 2 - y - 1;

	return hit(src, y, x);
}