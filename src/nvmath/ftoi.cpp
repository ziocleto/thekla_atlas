//
// Created by JFDP on 04/10/2018.
//

#include "ftoi.h"

int nv::ftoi_round_xs( double val, double magic ) {
#if 1
        DoubleAnd64 dunion;
        dunion.d = val + magic;
        return (int32) dunion.i; // just cast to grab the bottom bits
#else
        val += magic;
        return ((int*)&val)[0]; // @@ Assumes little endian.
#endif
}

int nv::ftoi_round_xs( float val ) {
        return ftoi_round_xs(val, floatutil_xs_doublemagic);
}

int nv::ftoi_floor_xs( float val ) {
        return ftoi_round_xs(val - floatutil_xs_doublemagicroundeps, floatutil_xs_doublemagic);
}

int nv::ftoi_ceil_xs( float val ) {
        return ftoi_round_xs(val + floatutil_xs_doublemagicroundeps, floatutil_xs_doublemagic);
}

int nv::ftoi_trunc_xs( float val ) {
        return (val<0) ? ftoi_ceil_xs(val) : ftoi_floor_xs(val);
}

#if NV_CPU_X86 || NV_CPU_X86_64

int nv::ftoi_round_sse( float f ) {
        return _mm_cvt_ss2si(_mm_set_ss(f));
}

int nv::ftoi_trunc_sse( float f ) {
        return _mm_cvtt_ss2si(_mm_set_ss(f));
}

#endif

#if NV_USE_SSE

int nv::ftoi_round( float val ) {
        return ftoi_round_sse(val);
}

int nv::ftoi_trunc( float f ) {
        return ftoi_trunc_sse(f);
}

int nv::ftoi_floor( float val ) {
        return ftoi_round(floorf(val));
}

int nv::ftoi_ceil( float val ) {
        return ftoi_round(ceilf(val));
}

void nv::test_ftoi() {

        // Round to nearest integer.
        nvCheck(ftoi_round(0.1f) == 0);
        nvCheck(ftoi_round(0.6f) == 1);
        nvCheck(ftoi_round(-0.2f) == 0);
        nvCheck(ftoi_round(-0.7f) == -1);
        nvCheck(ftoi_round(10.1f) == 10);
        nvCheck(ftoi_round(10.6f) == 11);
        nvCheck(ftoi_round(-90.1f) == -90);
        nvCheck(ftoi_round(-90.6f) == -91);

        nvCheck(ftoi_round(0) == 0);
        nvCheck(ftoi_round(1) == 1);
        nvCheck(ftoi_round(-1) == -1);

        nvCheck(ftoi_round(0.5f) == 0);  // How are midpoints rounded? Bankers rounding.
        nvCheck(ftoi_round(1.5f) == 2);
        nvCheck(ftoi_round(2.5f) == 2);
        nvCheck(ftoi_round(3.5f) == 4);
        nvCheck(ftoi_round(4.5f) == 4);
        nvCheck(ftoi_round(-0.5f) == 0);
        nvCheck(ftoi_round(-1.5f) == -2);


        // Truncation (round down if > 0, round up if < 0).
        nvCheck(ftoi_trunc(0.1f) == 0);
        nvCheck(ftoi_trunc(0.6f) == 0);
        nvCheck(ftoi_trunc(-0.2f) == 0);
        nvCheck(ftoi_trunc(-0.7f) == 0);    // @@ When using /arch:SSE2 in Win32, msvc produce wrong code for this one. It is skipping the addition.
        nvCheck(ftoi_trunc(1.99f) == 1);
        nvCheck(ftoi_trunc(-1.2f) == -1);

        // Floor (round down).
        nvCheck(ftoi_floor(0.1f) == 0);
        nvCheck(ftoi_floor(0.6f) == 0);
        nvCheck(ftoi_floor(-0.2f) == -1);
        nvCheck(ftoi_floor(-0.7f) == -1);
        nvCheck(ftoi_floor(1.99f) == 1);
        nvCheck(ftoi_floor(-1.2f) == -2);

        nvCheck(ftoi_floor(0) == 0);
        nvCheck(ftoi_floor(1) == 1);
        nvCheck(ftoi_floor(-1) == -1);
        nvCheck(ftoi_floor(2) == 2);
        nvCheck(ftoi_floor(-2) == -2);

        // Ceil (round up).
        nvCheck(ftoi_ceil(0.1f) == 1);
        nvCheck(ftoi_ceil(0.6f) == 1);
        nvCheck(ftoi_ceil(-0.2f) == 0);
        nvCheck(ftoi_ceil(-0.7f) == 0);
        nvCheck(ftoi_ceil(1.99f) == 2);
        nvCheck(ftoi_ceil(-1.2f) == -1);

        nvCheck(ftoi_ceil(0) == 0);
        nvCheck(ftoi_ceil(1) == 1);
        nvCheck(ftoi_ceil(-1) == -1);
        nvCheck(ftoi_ceil(2) == 2);
        nvCheck(ftoi_ceil(-2) == -2);
}

int nv::iround( float f ) {
        return ftoi_round(f);
        //return int(floorf(f + 0.5f));
}

int nv::iround( double f ) {
        return int(::floor(f + 0.5));
}

int nv::ifloor( float f ) {
        return ftoi_floor(f);
        //return int(floorf(f));
}

int nv::iceil( float f ) {
        return int(ceilf(f));
}

float nv::quantizeFloat( float x, uint bits, bool exactEndPoints ) {
        nvDebugCheck(bits <= 16);

        float range = float(1 << bits);
        if (exactEndPoints) {
                return floorf(x * (range-1) + 0.5f) / (range-1);
        }
        else {
                return (floorf(x * range) + 0.5f) / range;
        }
}

uint8 nv::unitFloatToFixed8( float f ) {
        return (uint8)unitFloatToFixed<8>(f);
}

uint16 nv::unitFloatToFixed16( float f ) {
        return (uint16)unitFloatToFixed<16>(f);
}

#endif
