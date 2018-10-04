// This code is in the public domain -- castano@gmail.com

#pragma once
#ifndef NV_MATH_FTOI_H
#define NV_MATH_FTOI_H

#include "nvmath/nvmath.h"

#include <math.h>

namespace nv
{
    // Optimized float to int conversions. See:
    // http://cbloomrants.blogspot.com/2009/01/01-17-09-float-to-int.html
    // http://www.stereopsis.com/sree/fpu2006.html
    // http://assemblyrequired.crashworks.org/2009/01/12/why-you-should-never-cast-floats-to-ints/
    // http://chrishecker.com/Miscellaneous_Technical_Articles#Floating_Point


    union DoubleAnd64 {
        uint64    i;
        double    d;
    };

    static const double floatutil_xs_doublemagic = (6755399441055744.0);                            // 2^52 * 1.5
    static const double floatutil_xs_doublemagicdelta = (1.5e-8);                                   // almost .5f = .5f + 1e^(number of exp bit)
    static const double floatutil_xs_doublemagicroundeps = (0.5f - floatutil_xs_doublemagicdelta);  // almost .5f = .5f - 1e^(number of exp bit)

     int ftoi_round_xs(double val, double magic);

     int ftoi_round_xs(float val);

     int ftoi_floor_xs(float val);

     int ftoi_ceil_xs(float val);

     int ftoi_trunc_xs(float val);

#if NV_CPU_X86 || NV_CPU_X86_64

     int ftoi_round_sse(float f);

     int ftoi_trunc_sse(float f);

#endif



#if NV_USE_SSE

     int ftoi_round(float val);

     int ftoi_trunc(float f);

    // We can probably do better than this. See for example:
    // http://dss.stephanierct.com/DevBlog/?p=8
     int ftoi_floor(float val);

     int ftoi_ceil(float val);

#else

    // In theory this should work with any double floating point math implementation, but it appears that MSVC produces incorrect code
    // when SSE2 is targeted and fast math is enabled (/arch:SSE2 & /fp:fast). These problems go away with /fp:precise, which is the default mode.

     int ftoi_round(float val) {
        return ftoi_round_xs(val);
    }

     int ftoi_floor(float val) {
        return ftoi_floor_xs(val);
    }

     int ftoi_ceil(float val) {
        return ftoi_ceil_xs(val);
    }

     int ftoi_trunc(float f) {
      return ftoi_trunc_xs(f);
    }

#endif


    inline void test_ftoi();





    // Safe versions using standard casts.

    inline int iround(float f);

    inline int iround(double f);

    inline int ifloor(float f);

    inline int iceil(float f);



    // I'm always confused about which quantizer to use. I think we should choose a quantizer based on how the values are expanded later and this is generally using the 'exact endpoints' rule.
    // Some notes from cbloom: http://cbloomrants.blogspot.com/2011/07/07-26-11-pixel-int-to-float-options.html

    // Quantize a float in the [0,1] range, using exact end points or uniform bins.
    inline float quantizeFloat(float x, uint bits, bool exactEndPoints = true);


    // This is the most common rounding mode:
    // 
    //   0     1       2     3
    // |___|_______|_______|___|
    // 0                       1
    //
    // You get that if you take the unit floating point number multiply by 'N-1' and round to nearest. That is, `i = round(f * (N-1))`.
    // You reconstruct the original float dividing by 'N-1': `f = i / (N-1)`


    //    0     1     2     3
    // |_____|_____|_____|_____|
    // 0                       1

    /*enum BinningMode {
        RoundMode_ExactEndPoints,       
        RoundMode_UniformBins,
    };*/

    template <int N>
    inline uint unitFloatToFixed(float f) {
        return ftoi_round(f * ((1<<N)-1));
    }

    inline uint8 unitFloatToFixed8(float f);

    inline uint16 unitFloatToFixed16(float f);


} // nv

#endif // NV_MATH_FTOI_H
