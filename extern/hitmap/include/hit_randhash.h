#ifndef _HitRandHash_
#define _HitRandHash_

#include<stdio.h>
#include<stdlib.h>

/**
 * High-quality random hash of an integer into several numeric types.
 *
 * Copyright (C) Numerical Recipes Software 1986-2007
 *
 * Updated to Hitmap v1.2
 * (c) 2015 Arturo Gonzalez-Escribano
 */
inline unsigned long HitRandhash_uint64(const unsigned long u) {
    unsigned long v = u * 3935559000370003845L + 2691343689449507681L;
    v ^= v >> 21; v ^= v << 37; v ^= v >> 4;
    v *= 4768777513237032717L;
    v ^= v << 20; v ^= v >> 41; v ^= v << 5;
    return  v;
  }

inline unsigned int HitRandhash_uint32(const unsigned long u){
    return (unsigned int)(HitRandhash_uint64(u) & 0xffffffffL);
  }

inline double HitRandhash_double(const unsigned long u){
    double r = 5.42101086242752217E-20 * HitRandhash_uint64(u);
    return r < 0 ? 1+r : r;
  }

inline int HitRandhash_int32(const unsigned long u) { return HitRandhash_uint32(u) & 0x7FFFFFFF; }

inline long HitRandhash_int64(const unsigned long u) { return HitRandhash_uint64(u) & 0x7FFFFFFFFFFFFFFFL; }

#endif
