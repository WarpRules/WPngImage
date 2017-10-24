/* This is a comprehensive test file for WPngImage.
   Do not include this file in your project. It's intended only to
   test modifications to the library.
*/

#ifndef TEST_DO_NOT_PRINT_WARNING
#warning "This is a test file. Do not include it in your project."
#endif

#include "WPngImage.hh"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cerrno>

typedef WPngImage::Byte Byte;
typedef WPngImage::UInt16 UInt16;
typedef WPngImage::Float Float;

static const char* const kTestPngImageFileName = "WPngImage_testing.png";

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

//============================================================================
// Pixel and color ostream output operators
//============================================================================
static std::ostream& operator<<(std::ostream& os, const WPngImage::Pixel8& p)
{ return os << "(" << int(p.r) << ", " << int(p.g) << ", " << int(p.b) << ", " << int(p.a) << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::Pixel16& p)
{ return os << "(" << p.r << ", " << p.g << ", " << p.b << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::PixelF& p)
{ return os << "(" << p.r << ", " << p.g << ", " << p.b << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::HSV& p)
{ return os << "(" << p.h << ", " << p.s << ", " << p.v << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::HSL& p)
{ return os << "(" << p.h << ", " << p.s << ", " << p.l << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::XYZ& p)
{ return os << "(" << p.x << ", " << p.y << ", " << p.z << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::YXY& p)
{ return os << "(" << p.Y << ", " << p.x << ", " << p.y << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::CMY& p)
{ return os << "(" << p.c << ", " << p.m << ", " << p.y << ", " << p.a << ")"; }
static std::ostream& operator<<(std::ostream& os, const WPngImage::CMYK& p)
{ return os << "(" << p.c << ", " << p.m << ", " << p.y << ", " << p.k << ", " << p.a << ")"; }


//============================================================================
// Pixel/color comparison functions
//============================================================================
static bool compareV(Byte v1, Byte v2) { return v1 == v2; }
static bool compareV(UInt16 v1, UInt16 v2) { return v1 == v2; }
static bool compareV(Float f1, Float f2) { return std::fabs(f1 - f2) < 0.001; }

static bool compareColF(Float f1, Float f2)
{
    return std::fabs(f1 - f2) < 0.01;
}

static int eType(Byte v) { return int(v); }
static int eType(UInt16 v) { return int(v); }
static int eType(int v) { return v; }
static unsigned eType(unsigned v) { return v; }
static Float eType(Float v) { return v; }

template<typename Pixel_t, typename Component_t>
static bool compare(int line, const Pixel_t& pixel,
                    Component_t r, Component_t g, Component_t b, Component_t a)
{
    if(!compareV(pixel.r, r) || !compareV(pixel.g, g) ||
       !compareV(pixel.b, b) || !compareV(pixel.a, a))
    {
        std::cout << "At line " << line << ": Pixel has value " << pixel
                  << " instead of (" << eType(r) << ", " << eType(g) << ", " << eType(b)
                  << ", " << eType(a) << ")\n";
        return false;
    }
    return true;
}

template<typename Pixel_t>
static bool compare(int line, const Pixel_t& pixel1, const Pixel_t& pixel2)
{
    if(!compareV(pixel1.r, pixel2.r) || !compareV(pixel1.g, pixel2.g) ||
       !compareV(pixel1.b, pixel2.b) || !compareV(pixel1.a, pixel2.a))
    {
        std::cout << "At line " << line << ": Pixel has value " << pixel1
                  << " instead of " << pixel2 << "\n";
        return false;
    }
    return true;
}

template<typename Component_t>
static bool compareComponents(int line, Component_t c1, Component_t c2)
{
    if(!compareV(c1, c2))
    {
        std::cout << "At line " << line << ": Value is " << eType(c1)
                  << " instead of " << eType(c2) << "\n";
        return false;
    }
    return true;
}

#define COMPFUNC(Type, m1, m2, m3, m4) \
    static bool compare(int line, const WPngImage::Type& c1, const WPngImage::Type& c2) { \
    if(!compareColF(c1.m1, c2.m1) || !compareColF(c1.m2, c2.m2) || \
       !compareColF(c1.m3, c2.m3) || !compareColF(c1.m4, c2.m4) || !compareColF(c1.a, c2.a)) { \
        std::cout << "At line " << line << ": Color has value " << c1 \
                  << " instead of " << c2 << "\n"; \
        return false; } return true; }

COMPFUNC(HSV, h, s, v, a)
COMPFUNC(HSL, h, s, l, a)
COMPFUNC(XYZ, x, y, z, a)
COMPFUNC(YXY, Y, x, y, a)
COMPFUNC(CMY, c, m, y, a)
COMPFUNC(CMYK, c, m, y, k)

#define COMPARE(pixel, r, g, b, a) if(!compare(__LINE__, pixel, r, g, b, a)) return false
#define COMPAREP(pixel1, pixel2) if(!compare(__LINE__, pixel1, pixel2)) return false
#define COMPARECOL(c1, c2) compare(__LINE__, c1, c2)
#define COMPARECOMP(c1, c2) if(!compareComponents(__LINE__, c1, c2)) return false
#define ERRORRET do { std::cout << "Called from line " << __LINE__ << "\n"; return false; } while(0)
#define ERRORRET1 do { std::cout << "Called from line " << __LINE__ << "\n"; return 1; } while(0)


//============================================================================
// Test basic pixel construction
//============================================================================
template<typename Pixel_t, typename CT>
static bool testPixelConstructors(CT maxAlpha)
{
    Pixel_t p1(123);
    COMPARE(p1, CT(123), CT(123), CT(123), maxAlpha);

    p1.set(234);
    COMPARE(p1, CT(234), CT(234), CT(234), maxAlpha);

    Pixel_t p2(12, 34);
    COMPARE(p2, 12, 12, 12, 34);

    p2.set(23, 45);
    COMPARE(p2, 23, 23, 23, 45);

    p2.set(67);
    COMPARE(p2, 67, 67, 67, 45);

    Pixel_t p3(1, 2, 3);
    COMPARE(p3, CT(1), CT(2), CT(3), maxAlpha);

    p3.set(2, 3, 4);
    COMPARE(p3, CT(2), CT(3), CT(4), maxAlpha);

    Pixel_t p4(10, 20, 30, 40);
    COMPARE(p4, 10, 20, 30, 40);

    p4.set(20, 30, 40, 50);
    COMPARE(p4, 20, 30, 40, 50);

    p4.set(35, 45, 55);
    COMPARE(p4, 35, 45, 55, 50);

    return true;
}

static bool testPixelConstruction()
{
    if(!testPixelConstructors<WPngImage::Pixel8>(255)) ERRORRET;
    if(!testPixelConstructors<WPngImage::Pixel16>(65535)) ERRORRET;
    if(!testPixelConstructors<WPngImage::PixelF>(1.0f)) ERRORRET;

    const WPngImage::Pixel8 p8(1, 2, 3);
    COMPARE(p8, 1, 2, 3, 255);

    WPngImage::Pixel16 p16(10, 20, 30);
    COMPARE(p16, 10, 20, 30, 65535);

    p16.r = 100; p16.g = 200; p16.b = 300; p16.a = 400;
    COMPARE(p16, 100, 200, 300, 400);

    const WPngImage::PixelF pF(0.5f, 0.75f, 1.0f, 0.25f);
    COMPARE(pF, 0.5f, 0.75f, 1.0f, 0.25f);

    const WPngImage::Pixel16 p16_2(p8);
    COMPARE(p16_2, 257, 514, 771, 65535);

    p16 = WPngImage::Pixel16(pF);
    COMPARE(p16, 32768, 49151, 65535, 16384);

    const WPngImage::Pixel8 p8_2(pF);
    COMPARE(p8_2, 128, 191, 255, 64);

    for(unsigned i = 0; i < 65536; ++i)
    {
        const WPngImage::Pixel16 p16(i, i, i, i);
        COMPARE(p16, i, i, i, i);
        const WPngImage::Pixel8 p8(p16);
        COMPARE(p8, i>>8, i>>8, i>>8, i>>8);
    }

    for(unsigned i = 0; i < 256; ++i)
    {
        const WPngImage::Pixel8 p8(i, 255 - i, i, 128 + i);
        const WPngImage::Pixel16 p16(p8);
        const WPngImage::Pixel8 p8_2(p16);
        const WPngImage::PixelF pF(p8);
        assert(p8 == p8_2);
        assert(!(p8 != p8_2));
        assert(p8 == WPngImage::Pixel8(p16));
        assert(p8 == WPngImage::Pixel8(pF));
    }

    return true;
}


//============================================================================
// Test integer pixel arithmetic operators
//============================================================================
template<typename Pixel_t>
static bool testPixelOperators1()
{
    //------------------------------------------------------------------------
    // Pixel - int operators
    //------------------------------------------------------------------------
    Pixel_t p1(0, 1, 2, 3), p2(10, 20, 30, 40);
    Pixel_t p = p1;

    p += 10;
    COMPARE(p, 10, 11, 12, 3);

    p = p2 + 20;
    COMPARE(p, 30, 40, 50, 40);

    const int maxValue = Pixel_t::kComponentMaxValue;

    p = p1 + (maxValue - 1);
    COMPARE(p, maxValue - 1, maxValue, maxValue, 3);

    p = 30 + p1;
    COMPARE(p, 30, 31, 32, 3);

    p -= 31;
    COMPARE(p, 0, 0, 1, 3);

    p = p2 - 15;
    COMPARE(p, 0, 5, 15, 40);

    p = 25 - p2;
    COMPARE(p, 15, 5, 0, 40);

    p *= 3;
    COMPARE(p, 45, 15, 0, 40);

    p = p1 * 5;
    COMPARE(p, 0, 5, 10, 3);

    p = 6 * p1;
    COMPARE(p, 0, 6, 12, 3);

    p = Pixel_t(maxValue / 2, maxValue / 3, maxValue / 4) * 3;
    COMPARE(p, maxValue, maxValue / 3 * 3, maxValue / 4 * 3, maxValue);

    p = p2;
    p /= 6;
    COMPARE(p, 2, 3, 5, 40);

    p = p2 / 10;
    COMPARE(p, 1, 2, 3, 40);

    p = p2 / 0;
    COMPARE(p, maxValue, maxValue, maxValue, 40);

    p = 200 / p1;
    COMPARE(p, maxValue, 200, 100, 3);

    //------------------------------------------------------------------------
    // Pixel - pixel operators
    //------------------------------------------------------------------------
    p1.set(50, 100, 150, 100);
    p2.set(10, 20, 30, 200);
    p = p1 + p2;
    COMPARE(p, 60, 120, 180, 150);

    p += Pixel_t(5, 15, 25, 50);
    COMPARE(p, 65, 135, 205, 100);

    p = p1 - p2;
    COMPARE(p, 40, 80, 120, 150);

    p -= Pixel_t(5, 15, 25, 50);
    COMPARE(p, 35, 65, 95, 100);

    p *= Pixel_t(5, 3, 2, 50);
    COMPARE(p, 175, 195, 190, 75);

    p = Pixel_t(1, 2, 3, 10) * Pixel_t(10, 20, 30, 20);
    COMPARE(p, 10, 40, 90, 15);

    p /= Pixel_t(2, 4, 3, 25);
    COMPARE(p, 5, 10, 30, 20);

    p = Pixel_t(10, 20, 30, 40) / Pixel_t(5, 10, 0, 80);
    COMPARE(p, 2, 2, maxValue, 60);

    return true;
}


//============================================================================
// Test pixel blending
//============================================================================
template<typename Pixel_t>
static bool testBlending(Pixel_t dest1, Pixel_t dest2, Pixel_t src, Pixel_t res1, Pixel_t res2)
{
    Pixel_t p = dest1.blendedPixel(src);
    COMPARE(p, res1.r, res1.g, res1.b, res1.a);

    p = dest2.blendedPixel(src);
    COMPARE(p, res2.r, res2.g, res2.b, res2.a);

    p = dest1;
    p.blendWith(src);
    COMPARE(p, res1.r, res1.g, res1.b, res1.a);

    p = dest2;
    p.blendWith(src);
    COMPARE(p, res2.r, res2.g, res2.b, res2.a);

    return true;
}

//============================================================================
// Test pixel interpolation
//============================================================================
template<typename Pixel_t>
struct InterpolationTestData
{
    Pixel_t p1, p2, result, rawResult;
    typename Pixel_t::Component_t factor;
};

template<typename Pixel_t>
static bool testInterpolation(Pixel_t p1, Pixel_t p2, Pixel_t result, Pixel_t rawResult,
                              typename Pixel_t::Component_t factor)
{
#define COMPARE_INTERP(comp1, comp2) \
    do { if(!compare(__LINE__, comp1, comp2)) { \
        std::cout << " -> p1=" << p1 << ", p2=" << p2 << ", factor=" << eType(factor) << "\n"; \
        return false; } } while(0)

    Pixel_t testP1 = p1;
    const Pixel_t testP2 = p1.interpolatedPixel(p2, factor);
    Pixel_t testP3 = p1;
    const Pixel_t testP4 = p1.rawInterpolatedPixel(p2, factor);
    testP1.interpolate(p2, factor);
    testP3.rawInterpolate(p2, factor);
    COMPARE_INTERP(testP1, result);
    COMPARE_INTERP(testP2, result);
    COMPARE_INTERP(testP3, rawResult);
    COMPARE_INTERP(testP4, rawResult);
    return true;

#undef COMPARE_INTERP
}

template<typename Pixel_t, std::size_t kAmount>
static bool testInterpolation(const InterpolationTestData<Pixel_t>(&testData)[kAmount],
                              typename Pixel_t::Component_t maxComponentValue)
{
    for(std::size_t i = 0; i < kAmount; ++i)
    {
        if(!testInterpolation(testData[i].p1, testData[i].p2,
                              testData[i].result, testData[i].rawResult,
                              testData[i].factor)) ERRORRET;

        // If the alphas are the same, we can do the same test with a range of alphas:
        if(testData[i].p1.a == testData[i].p2.a)
        {
            const typename Pixel_t::Component_t step = maxComponentValue / 10;
            for(typename Pixel_t::Component_t alpha = 0; alpha < maxComponentValue; alpha += step)
            {
                Pixel_t p1 = testData[i].p1, p2 = testData[i].p2,
                    res = testData[i].result, rawRes = testData[i].rawResult;
                p1.a = alpha; p2.a = alpha; res.a = alpha; rawRes.a = alpha;
                if(!testInterpolation(p1, p2, res, rawRes, testData[i].factor)) ERRORRET;
            }
        }
    }
    return true;
}

template<typename Pixel_t, std::size_t kFactorsAmount>
static bool testTrivialInterpolation
(const typename Pixel_t::Component_t(&factors)[kFactorsAmount],
 Pixel_t additionalTestPixel)
{
    for(unsigned factorInd = 0; factorInd < kFactorsAmount; ++factorInd)
    {
        for(unsigned alphaInd = 0; alphaInd < kFactorsAmount; ++alphaInd)
        {
            const typename Pixel_t::Component_t alpha = factors[alphaInd];
            for(unsigned compInd = 0; compInd < kFactorsAmount; ++compInd)
            {
                const typename Pixel_t::Component_t comp = factors[alphaInd];
                const Pixel_t pixel(comp, comp, comp, alpha);
                if(!testInterpolation
                   (pixel, pixel, pixel, pixel, factors[factorInd])) ERRORRET;
            }
            additionalTestPixel.a = alpha;
            if(!testInterpolation
               (additionalTestPixel, additionalTestPixel,
                additionalTestPixel, additionalTestPixel,
                factors[factorInd])) ERRORRET;
        }
    }
    return true;
}

static bool testInterpolation()
{
    typedef WPngImage::Pixel8 P8;
    typedef WPngImage::Pixel16 P16;
    typedef WPngImage::PixelF PF;

    const P8::Component_t factors8[] = { 0, 1, 10, 64, 100, 127, 128, 200, 254, 255 };
    const P16::Component_t factors16[] = { 0, 1, 100, 5000, 32767, 32768, 50000, 65534, 65535 };
    const PF::Component_t factorsF[] = { 0.0, 0.1, 0.25, 0.49, 0.5, 0.76, 0.99, 1.0 };

    if(!testTrivialInterpolation(factors8, P8(255, 128, 64))) ERRORRET;
    if(!testTrivialInterpolation(factors16, P16(65535, 32768, 16384))) ERRORRET;
    if(!testTrivialInterpolation(factorsF, PF(1.0, 0.5, 0.25))) ERRORRET;

    const InterpolationTestData<P8> kTestData8[] =
    {
        { P8(255, 128, 64, 255), P8(0, 0, 0, 255),
          P8(255, 128, 64, 255), P8(255, 128, 64, 255), 0 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 255),
          P8(127, 63, 31, 255), P8(127, 63, 31, 255), 128 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 255),
          P8(0, 0, 0, 255), P8(0, 0, 0, 255), 255 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 255),
          P8(63, 31, 15, 255), P8(63, 31, 15, 255), 192 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 255),
          P8(191, 95, 47, 255), P8(191, 95, 47, 255), 64 },

        { P8(255, 128, 64, 255), P8(10, 20, 30, 0),
          P8(255, 128, 64, 255), P8(255, 128, 64, 255), 0 },
        { P8(255, 128, 64, 255), P8(40, 50, 60, 0),
          P8(255, 128, 64, 127), P8(147, 88, 61, 127), 128 },
        { P8(255, 128, 64, 255), P8(70, 80, 90, 0),
          P8(0, 0, 0, 0), P8(70, 80, 90, 0), 255 },

        { P8(255, 128, 64, 255), P8(0, 0, 0, 128),
          P8(255, 128, 64, 255), P8(255, 128, 64, 255), 0 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 128),
          P8(169, 84, 42, 191), P8(127, 63, 31, 191), 128 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 128),
          P8(0, 0, 0, 128), P8(0, 0, 0, 128), 255 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 128),
          P8(100, 50, 25, 159), P8(63, 31, 15, 159), 192 },
        { P8(255, 128, 64, 255), P8(0, 0, 0, 128),
          P8(218, 109, 54, 223), P8(191, 95, 47, 223), 64 },

        { P8(192, 228, 48, 200), P8(60, 120, 160, 150),
          P8(167, 207, 69, 188), P8(160, 202, 74, 188), 60 }
    };

    const InterpolationTestData<P16> kTestData16[] =
    {
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 65535),
          P16(65535, 32768, 16384, 65535), P16(65535, 32768, 16384, 65535), 0 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 65535),
          P16(32767, 16383, 8191, 65535), P16(32767, 16383, 8191, 65535), 32768 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 65535),
          P16(0, 0, 0, 65535), P16(0, 0, 0, 65535), 65535 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 65535),
          P16(16383, 8191, 4095, 65535), P16(16383, 8191, 4095, 65535), 49152 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 65535),
          P16(49151, 24575, 12287, 65535), P16(49151, 24575, 12287, 65535), 16384 },

        { P16(65535, 32768, 16384, 65535), P16(1000, 2000, 3000, 0),
          P16(65535, 32768, 16384, 65535), P16(65535, 32768, 16384, 65535), 0 },
        { P16(65535, 32768, 16384, 65535), P16(4000, 5000, 6000, 0),
          P16(65535, 32768, 16384, 32767), P16(34767, 18883, 11191, 32767), 32768 },
        { P16(65535, 32768, 16384, 65535), P16(7000, 8000, 9000, 0),
          P16(0, 0, 0, 0), P16(7000, 8000, 9000, 0), 65535 },

        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 32768),
          P16(65535, 32768, 16384, 65535), P16(65535, 32768, 16384, 65535), 0 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 32768),
          P16(43689, 21844, 10922, 49151), P16(32767, 16383, 8191, 49151), 32768 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 32768),
          P16(0, 0, 0, 32768), P16(0, 0, 0, 32768), 65535 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 32768),
          P16(26212, 13106, 6553, 40959), P16(16383, 8191, 4095, 40959), 49152 },
        { P16(65535, 32768, 16384, 65535), P16(0, 0, 0, 32768),
          P16(56172, 28086, 14043, 57343), P16(49151, 24575, 12287, 57343), 16384 },
    };

    const InterpolationTestData<PF> kTestDataF[] =
    {
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 1.0),
          PF(1.0, 0.5, 0.25, 1.0), PF(1.0, 0.5, 0.25, 1.0), 0.0 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 1.0),
          PF(0.5, 0.25, 0.125, 1.0), PF(0.5, 0.25, 0.125, 1.0), 0.5 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 1.0),
          PF(0, 0, 0, 1.0), PF(0, 0, 0, 1.0), 1.0 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 1.0),
          PF(0.75, 0.375, 0.1875, 1.0), PF(0.75, 0.375, 0.1875, 1.0), 0.25 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 1.0),
          PF(0.25, 0.125, 0.0625, 1.0), PF(0.25, 0.125, 0.0625, 1.0), 0.75 },

        { PF(1.0, 0.5, 0.25, 1.0), PF(0.1, 0.2, 0.3, 0.0),
          PF(1.0, 0.5, 0.25, 1.0), PF(1.0, 0.5, 0.25, 1.0), 0.0 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0.1, 0.2, 0.3, 0.0),
          PF(1.0, 0.5, 0.25, 0.5), PF(0.55, 0.35, 0.275, 0.5), 0.5 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0.1, 0.2, 0.3, 0.0),
          PF(0, 0, 0, 0), PF(0.1, 0.2, 0.3, 0), 1.0 },

        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 0.5),
          PF(1.0, 0.5, 0.25, 1.0), PF(1.0, 0.5, 0.25, 1.0), 0.0 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 0.25),
          PF(0.8, 0.4, 0.2, 0.625), PF(0.5, 0.25, 0.125, 0.625), 0.5 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 0.25),
          PF(0, 0, 0, 0.25), PF(0, 0, 0, 0.25), 1.0 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 0.5),
          PF(0.4, 0.2, 0.1, 0.625), PF(0.25, 0.125, 0.0625, 0.625), 0.75 },
        { PF(1.0, 0.5, 0.25, 1.0), PF(0, 0, 0, 0.75),
          PF(0.8, 0.4, 0.2, 0.9375), PF(0.75, 0.375, 0.1875, 0.9375), 0.25 },
    };

    if(!testInterpolation(kTestData8, 255)) ERRORRET;
    if(!testInterpolation(kTestData16, 65535)) ERRORRET;
    if(!testInterpolation(kTestDataF, 1.0)) ERRORRET;

    return true;
}

//============================================================================
// Test pixel averaging
//============================================================================
template<typename Pixel_t>
static bool testAveraging(Pixel_t p1, Pixel_t p2, Pixel_t p3,
                          Pixel_t res1, Pixel_t res2, Pixel_t res3)
{
    Pixel_t p = p1;
    p.averageWith(p2);
    COMPARE(p, res1.r, res1.g, res1.b, res1.a);

    p = p1.averagedPixel(p2);
    COMPARE(p, res1.r, res1.g, res1.b, res1.a);

    p = p2;
    p.averageWith(p3);
    COMPARE(p, res2.r, res2.g, res2.b, res2.a);

    p = p2.averagedPixel(p3);
    COMPARE(p, res2.r, res2.g, res2.b, res2.a);

#if !WPNGIMAGE_RESTRICT_TO_CPP98
    p = p1;
    p.averageWith(p2, p3);
    COMPARE(p, res3.r, res3.g, res3.b, res3.a);

    p = p1.averagedPixel(p2, p3);
    COMPARE(p, res3.r, res3.g, res3.b, res3.a);
#endif

    return res3.r + 256 != 0;
}

template<typename Pixel_t>
static bool testAveraging1()
{
    if(!testAveraging(Pixel_t(0, 0, 0, 0),
                      Pixel_t(10, 20, 30, 0),
                      Pixel_t(20, 40, 80, 252),
                      Pixel_t(0, 0, 0, 0),
                      Pixel_t(20, 40, 80, 126),
                      Pixel_t(20, 40, 80, 84))) ERRORRET;

    if(!testAveraging(Pixel_t(0, 0, 0, 128),
                      Pixel_t(30, 60, 120, 140),
                      Pixel_t(20, 40, 80, 253),
                      Pixel_t(16, 31, 63, 134),
                      Pixel_t(24, 47, 94, 197),
                      Pixel_t(18, 36, 71, 174))) ERRORRET;

    if(!testAveraging(Pixel_t(15, 15, 28, 18),
                      Pixel_t(221, 22, 121, 100),
                      Pixel_t(2, 4, 8, 2),
                      Pixel_t(190, 21, 107, 59),
                      Pixel_t(217, 22, 119, 51),
                      Pixel_t(186, 21, 105, 40))) ERRORRET;

    return true;
}

static bool testAveraging2()
{
    if(!testAveraging(WPngImage::PixelF(0, 0, 0, 0),
                      WPngImage::PixelF(1.0, 0.5, 0.25, 0),
                      WPngImage::PixelF(0.25, 0.5, 1.0, 0.5),
                      WPngImage::PixelF(0, 0, 0, 0),
                      WPngImage::PixelF(0.25, 0.5, 1.0, 0.25),
                      WPngImage::PixelF(0.25, 0.5, 1.0, 0.5/3.0))) ERRORRET;

    if(!testAveraging(WPngImage::PixelF(0, 0, 0, 0.5),
                      WPngImage::PixelF(1.0, 0.5, 0.25, 0.75),
                      WPngImage::PixelF(0.25, 0.5, 1.0, 1.0),
                      WPngImage::PixelF(0.6, 0.3, 0.15, 0.625),
                      WPngImage::PixelF(0.571429, 0.5, 0.678571, 0.875),
                      WPngImage::PixelF(0.444444, 0.388888, 0.527777, 0.75))) ERRORRET;

    return true;
}


//============================================================================
// Test pixel conversion to grayscale
//============================================================================
template<typename Pixel_t>
static bool testGrayscale1()
{
    typedef typename Pixel_t::Component_t CT;

    Pixel_t p(0, 0, 0);
    COMPARECOMP(p.toGray(), CT(0));
    COMPAREP(p.grayPixel(), Pixel_t(0, 0, 0));

    p.set(100, 100, 100);
    COMPARECOMP(p.toGray(), CT(100));
    COMPAREP(p.grayPixel(), Pixel_t(100, 100, 100));

    p.set(12, 34, 56);
    COMPARECOMP(p.toGray(), CT(30));
    COMPAREP(p.grayPixel(), Pixel_t(30, 30, 30));

    p.set(34, 56, 78);
    COMPARECOMP(p.toGray(), CT(52));
    COMPAREP(p.grayPixel(), Pixel_t(52, 52, 52));

    p.set(255, 0, 255);
    COMPARECOMP(p.toGray(), CT(105));
    COMPAREP(p.grayPixel(), Pixel_t(105, 105, 105));

    return true;
}

static bool testGrayscale2()
{
    WPngImage::Pixel8 p8(0, 0, 0);
    WPngImage::Pixel16 p16(0, 0, 0);
    WPngImage::PixelF pF(0, 0, 0);
    COMPARECOMP(p8.toGrayCIE(), Byte(0));
    COMPARECOMP(p16.toGrayCIE(), UInt16(0));
    COMPARECOMP(pF.toGrayCIE(), 0.0f);
    COMPAREP(p8.grayCIEPixel(), WPngImage::Pixel8(0, 0, 0));
    COMPAREP(p16.grayCIEPixel(), WPngImage::Pixel16(0, 0, 0));
    COMPAREP(pF.grayCIEPixel(), WPngImage::PixelF(0, 0, 0));

    p8.set(100, 100, 100);
    p16.set(5000, 5000, 5000);
    pF.set(0.4, 0.4, 0.4);
    COMPARECOMP(p8.toGrayCIE(), Byte(100));
    COMPARECOMP(p16.toGrayCIE(), UInt16(5000));
    COMPARECOMP(pF.toGrayCIE(), 0.4f);
    COMPAREP(p8.grayCIEPixel(), WPngImage::Pixel8(100, 100, 100));
    COMPAREP(p16.grayCIEPixel(), WPngImage::Pixel16(5000, 5000, 5000));
    COMPAREP(pF.grayCIEPixel(), WPngImage::PixelF(0.4f, 0.4f, 0.4f));

    p8.set(10, 50, 150);
    p16.set(1000, 50000, 7000);
    pF.set(0.2, 0.4, 0.9);
    COMPARECOMP(p8.toGrayCIE(), Byte(60));
    COMPARECOMP(p16.toGrayCIE(), UInt16(43065));
    COMPARECOMP(pF.toGrayCIE(), 0.435237f);
    COMPAREP(p8.grayCIEPixel(), WPngImage::Pixel8(60, 60, 60));
    COMPAREP(p16.grayCIEPixel(), WPngImage::Pixel16(43065, 43065, 43065));
    COMPAREP(pF.grayCIEPixel(), WPngImage::PixelF(0.435237f, 0.435237f, 0.435237f));

    p8.set(255, 0, 255);
    p16.set(65535, 0, 65535);
    pF.set(1, 0, 1);
    COMPARECOMP(p8.toGrayCIE(), Byte(145));
    COMPARECOMP(p16.toGrayCIE(), UInt16(37364));
    COMPARECOMP(pF.toGrayCIE(), 0.57014f);
    COMPAREP(p8.grayCIEPixel(), WPngImage::Pixel8(145, 145, 145));
    COMPAREP(p16.grayCIEPixel(), WPngImage::Pixel16(37364, 37364, 37364));
    COMPAREP(pF.grayCIEPixel(), WPngImage::PixelF(0.57014f, 0.57014f, 0.57014f));

    return true;
}


//============================================================================
// Test float pixel arithmetic operators, blending, averaging and grayscale
//============================================================================
static bool testPixelOperators2()
{
    //------------------------------------------------------------------------
    // Pixel - float operators
    //------------------------------------------------------------------------
    WPngImage::PixelF p, p2;
    COMPARE(p, 0.0f, 0.0f, 0.0f, 1.0f);

    p.set(1.25f, 2.5f, 5.0f, 0.75f);
    COMPARE(p, 1.25f, 2.5f, 5.0f, 0.75f);

    p += 2.0;
    COMPARE(p, 3.25f, 4.5f, 7.0f, 0.75f);

    p2 = p + 0.2;
    COMPARE(p2, 3.45f, 4.7f, 7.2f, 0.75f);

    p2 = 0.2 + p;
    COMPARE(p2, 3.45f, 4.7f, 7.2f, 0.75f);

    p -= 3.0;
    COMPARE(p, 0.25f, 1.5f, 4.0f, 0.75f);

    p2 = p - 0.2f;
    COMPARE(p2, 0.05f, 1.3f, 3.8f, 0.75f);

    p2 = 1.0f - p;
    COMPARE(p2, 0.75f, -0.5f, -3.0f, 0.75f);

    p *= 2.0;
    COMPARE(p, 0.5f, 3.0f, 8.0f, 0.75f);

    p2 = p * 0.25;
    COMPARE(p2, 0.125f, 0.75f, 2.0f, 0.75f);

    p2 = 0.25 * p;
    COMPARE(p2, 0.125f, 0.75f, 2.0f, 0.75f);

    p /= 2.0;
    COMPARE(p, 0.25f, 1.5f, 4.0f, 0.75f);

    p2 = p / 10.0;
    COMPARE(p2, 0.025f, 0.15f, 0.4f, 0.75f);

    p2 = 10.0 / p;
    COMPARE(p2, 40.0f, 6.666666f, 2.5f, 0.75f);


    //------------------------------------------------------------------------
    // Pixel - pixel operators
    //------------------------------------------------------------------------
    p = WPngImage::PixelF(0.5f, 0.25f, 0.75f, 0.25f) + WPngImage::PixelF(0.1f, 0.2f, 0.3f, 0.75f);
    COMPARE(p, 0.6f, 0.45f, 1.05f, 0.5f);

    p += WPngImage::PixelF(0.2f, 0.3f, -0.5f, 1.0f);
    COMPARE(p, 0.8f, 0.75f, 0.55f, 0.75f);

    p = WPngImage::PixelF(0.5f, 0.25f, 0.75f, 0.25f) - WPngImage::PixelF(0.1f, 0.2f, 0.3f, 0.75f);
    COMPARE(p, 0.4f, 0.05f, 0.45f, 0.5f);

    p -= WPngImage::PixelF(0.2f, 0.3f, -0.5f, 1.0f);
    COMPARE(p, 0.2f, -0.25f, 0.95f, 0.75f);

    p = WPngImage::PixelF(0.5f, 0.25f, 0.75f, 0.25f) * WPngImage::PixelF(0.1f, 0.2f, 0.3f, 0.75f);
    COMPARE(p, 0.5f*0.1f, 0.25f*0.2f, 0.75f*0.3f, 0.5f);

    p *= WPngImage::PixelF(0.2f, 0.3f, -0.5f, 1.0f);
    COMPARE(p, 0.5f*0.1f*0.2f, 0.25f*0.2f*0.3f, 0.75f*0.3f*-0.5f, 0.75f);

    p = WPngImage::PixelF(0.5f, 0.25f, 0.75f, 0.25f) / WPngImage::PixelF(0.1f, 0.2f, 0.3f, 0.75f);
    COMPARE(p, 0.5f/0.1f, 0.25f/0.2f, 0.75f/0.3f, 0.5f);

    p /= WPngImage::PixelF(0.2f, 0.3f, -0.5f, 1.0f);
    COMPARE(p, 0.5f/0.1f/0.2f, 0.25f/0.2f/0.3f, 0.75f/0.3f/-0.5f, 0.75f);


    //------------------------------------------------------------------------
    // Pixel blending
    //------------------------------------------------------------------------
    if(!testBlending(WPngImage::Pixel8(0, 0, 0, 255),
                     WPngImage::Pixel8(0, 0, 0, 128),
                     WPngImage::Pixel8(255, 255, 255, 128),
                     WPngImage::Pixel8(128, 128, 128, 255),
                     WPngImage::Pixel8(170, 170, 170, 192))) ERRORRET;

    if(!testBlending(WPngImage::Pixel16(0, 0, 0, 65535),
                     WPngImage::Pixel16(0, 0, 0, 32768),
                     WPngImage::Pixel16(65535, 65535, 65535, 32768),
                     WPngImage::Pixel16(32768, 32768, 32768, 65535),
                     WPngImage::Pixel16(43690, 43690, 43690, 49152))) ERRORRET;

    if(!testBlending(WPngImage::PixelF(0, 0, 0, 1),
                     WPngImage::PixelF(0, 0, 0, 0.5),
                     WPngImage::PixelF(1, 1, 1, 0.5),
                     WPngImage::PixelF(0.5, 0.5, 0.5, 1),
                     WPngImage::PixelF(2.0/3.0, 2.0/3.0, 2.0/3.0, 0.75))) ERRORRET;


    //------------------------------------------------------------------------
    // Pixel interpolation
    //------------------------------------------------------------------------
    if(!testInterpolation()) ERRORRET;


    //------------------------------------------------------------------------
    // Pixel averaging and grayscale
    //------------------------------------------------------------------------
    if(!testAveraging1<WPngImage::Pixel8>()) ERRORRET;
    if(!testAveraging1<WPngImage::Pixel16>()) ERRORRET;
    if(!testAveraging2()) ERRORRET;

    if(!testGrayscale1<WPngImage::Pixel8>()) ERRORRET;
    if(!testGrayscale1<WPngImage::Pixel16>()) ERRORRET;
    if(!testGrayscale2()) ERRORRET;

    return true;
}


//============================================================================
// Test color space operations
//============================================================================
template<typename Color_t>
static bool testSettingFromColorSpace
(const Color_t color, Float v1, Float v2, Float v3, Float v4,
 const WPngImage::PixelF correctResult,
 void(WPngImage::PixelF::*setFunc)(Float, Float, Float, Float))
{
    WPngImage::PixelF rgba;
    rgba.set(color);
    COMPAREP(rgba, correctResult);
    (rgba.*setFunc)(v1, v2, v3, v4);
    COMPAREP(rgba, correctResult);
    const WPngImage::PixelF rgba2(color);
    COMPAREP(rgba2, correctResult);
    return true;
}

template<typename Color_t>
static bool testSettingFromColorSpace
(const Color_t color, Float v1, Float v2, Float v3, Float v4, Float v5,
 const WPngImage::PixelF correctResult,
 void(WPngImage::PixelF::*setFunc)(Float, Float, Float, Float, Float))
{
    WPngImage::PixelF rgba;
    rgba.set(color);
    COMPAREP(rgba, correctResult);
    (rgba.*setFunc)(v1, v2, v3, v4, v5);
    COMPAREP(rgba, correctResult);
    const WPngImage::PixelF rgba2(color);
    COMPAREP(rgba2, correctResult);
    return true;
}

static bool testColorSpaces()
{
    struct TestData
    {
        WPngImage::PixelF rgba;
        WPngImage::HSV hsv;
        WPngImage::HSL hsl;
        WPngImage::XYZ xyz;
        WPngImage::YXY yxy;
        WPngImage::CMY cmy;
        WPngImage::CMYK cmyk;
    };

    const TestData testData[] =
    {
        { WPngImage::PixelF(0, 0, 0, 0),
          { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
          { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
          { 1, 1, 1, 0 }, { 0, 0, 0, 1, 0 } },
        { WPngImage::PixelF(1, 1, 1, 1),
          { 0, 0, 1, 1 }, { 0, 0, 1, 1 },
          { 95.05, 100, 108.9, 1 }, { 100.0, 0.3127, 0.329, 1 },
          { 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 } },
        { WPngImage::PixelF(0.149, 0, 0, 0.1),
          { 0, 1, 0.149, 0.1 }, { 0, 1, 0.075, 0.1 },
          { 0.799, 0.417, 0.037, 0.1 }, { 0.412, 0.64, 0.33, 0.1 },
          { 0.851, 1, 1, 0.1 }, { 0, 1, 1, 0.851, 0.1 } },
        { WPngImage::PixelF(0.149, 0.53333, 0.898, 0.2),
          { 0.5807, 0.8333, 0.8941, 0.2 }, { 209.0/360.0, 0.786, 0.524, 0.2 },
          { 23.746, 23.678, 77.447, 0.2 }, { 23.677, 0.19, 0.1896, 0.2 },
          { 0.851, 0.46666, 0.10196, 0.2 }, { 0.83406, 0.40611, 0, 0.10196, 0.2 } },
        { WPngImage::PixelF(0.25, 0.75, 0.5, 0.3),
          { 0.4167, 0.665, 0.749, 0.3 }, { 0.4167, 0.498, 0.5, 0.3 },
          { 24.647, 39.998, 26.671, 0.3 }, { 40.0, 0.27, 0.438, 0.3 },
          { 0.75, 0.25, 0.5, 0.3 }, { 0.6666, 0, 0.3333, 0.25, 0.3 } },
    };

    for(unsigned i = 0; i < sizeof(testData)/sizeof(*testData); ++i)
    {
        const TestData& compData = testData[i];
        const TestData calcData =
        {
            compData.rgba, compData.rgba.toHSV(), compData.rgba.toHSL(),
            compData.rgba.toXYZ(), compData.rgba.toYXY(),
            compData.rgba.toCMY(), compData.rgba.toCMYK()
        };

#define ERRORRET_I do { std::cout << "Index value: " << i << "\n"; ERRORRET; } while(0)

        if(!COMPARECOL(calcData.hsv, compData.hsv)) ERRORRET_I;
        if(!COMPARECOL(calcData.hsl, compData.hsl)) ERRORRET_I;
        if(!COMPARECOL(calcData.xyz, compData.xyz)) ERRORRET_I;
        if(!COMPARECOL(calcData.yxy, compData.yxy)) ERRORRET_I;
        if(!COMPARECOL(calcData.cmy, compData.cmy)) ERRORRET_I;
        if(!COMPARECOL(calcData.cmyk, compData.cmyk)) ERRORRET_I;

        if(!testSettingFromColorSpace
           (calcData.hsv, calcData.hsv.h, calcData.hsv.s, calcData.hsv.v, calcData.hsv.a,
            compData.rgba, &WPngImage::PixelF::setFromHSV)) ERRORRET_I;
        if(!testSettingFromColorSpace
           (calcData.hsl, calcData.hsl.h, calcData.hsl.s, calcData.hsl.l, calcData.hsl.a,
            compData.rgba, &WPngImage::PixelF::setFromHSL)) ERRORRET_I;
        if(!testSettingFromColorSpace
           (calcData.xyz, calcData.xyz.x, calcData.xyz.y, calcData.xyz.z, calcData.xyz.a,
            compData.rgba, &WPngImage::PixelF::setFromXYZ)) ERRORRET_I;
        if(!testSettingFromColorSpace
           (calcData.yxy, calcData.yxy.Y, calcData.yxy.x, calcData.yxy.y, calcData.yxy.a,
            compData.rgba, &WPngImage::PixelF::setFromYXY)) ERRORRET_I;
        if(!testSettingFromColorSpace
           (calcData.cmy, calcData.cmy.c, calcData.cmy.m, calcData.cmy.y, calcData.cmy.a,
            compData.rgba, &WPngImage::PixelF::setFromCMY)) ERRORRET_I;
        if(!testSettingFromColorSpace
           (calcData.cmyk, calcData.cmyk.c, calcData.cmyk.m, calcData.cmyk.y, calcData.cmyk.k,
            calcData.cmy.a, compData.rgba, &WPngImage::PixelF::setFromCMYK)) ERRORRET_I;
    }

    return true;
}


//============================================================================
// Auxiliary functions for WPngImage tests
//============================================================================
class Rng
{
    unsigned mSeed;

 public:
    Rng(unsigned seed): mSeed(seed) {}
    unsigned operator()() { mSeed = mSeed * 529183901 + 12345; return mSeed >> 16; }
};

template<typename CT> static CT toCT(int);
template<> inline Byte toCT<Byte>(int value) { return Byte(value); }
template<> inline UInt16 toCT<UInt16>(int value) { return UInt16(value); }
template<> inline Float toCT<Float>(int value) { return value * (1.0/65535.0); }

static void getPixel(const WPngImage& image, int x, int y, WPngImage::Pixel8& dest)
{ dest = image.get8(x, y); }

static void getPixel(const WPngImage& image, int x, int y, WPngImage::Pixel16& dest)
{ dest = image.get16(x, y); }

static void getPixel(const WPngImage& image, int x, int y, WPngImage::PixelF& dest)
{ dest = image.getF(x, y); }

static const int kPatternsAmount = 4;


//============================================================================
// Test WPngImage creation and pixel operations
//============================================================================
template<typename Pixel_t>
static void setTestPattern(int index, WPngImage& dest, int width, int height)
{
    typedef typename Pixel_t::Component_t CT;

    if(index == 0)
    {
        dest.newImage(width, height, Pixel_t(0, 0, 0, 0));
        return;
    }
    if(index == 1)
    {
        const CT c = toCT<CT>(65535);
        dest.newImage(width, height, Pixel_t(c, c, c, c));
        return;
    }

    dest.newImage(width, height, Pixel_t());

    if(index == 2)
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
                dest.set(x, y, Pixel_t(toCT<CT>(x), toCT<CT>(y), toCT<CT>((x+y)/2),
                                       toCT<CT>(x/2 + y/3)));
    }
    else
    {
        Rng rng(1);
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
                dest.set(x, y, Pixel_t(toCT<CT>(rng()), toCT<CT>(rng()), toCT<CT>(rng()),
                                       toCT<CT>(rng())));
    }
}

template<typename Pixel_t>
static bool checkTestPattern(int index, const WPngImage& image, int width, int height)
{
    if(image.width() != width || image.height() != height)
    {
        std::cout << "Image has dimensions (" << image.width() << ", " << image.height()
                  << ") instead of (" << width << ", " << height << ").\n";
        ERRORRET;
    }

    typedef typename Pixel_t::Component_t CT;

#define COMPIMAGEPIXELS(p, r, g, b, a) do { \
        if(!compare(__LINE__, pixel, r, g, b, a)) {     \
            std::cout << " -> at image coordinates (" << x << ", " << y << ")\n"; \
            return false; } } while(0)

    if(index == 0 || index == 1)
    {
        const CT c = index == 0 ? 0 : toCT<CT>(65535);
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                Pixel_t pixel;
                getPixel(image, x, y, pixel);
                COMPIMAGEPIXELS(pixel, c, c, c, c);
            }
    }
    else if(index == 2)
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                Pixel_t pixel;
                getPixel(image, x, y, pixel);
                COMPIMAGEPIXELS(pixel, toCT<CT>(x), toCT<CT>(y), toCT<CT>((x+y)/2),
                                toCT<CT>(x/2 + y/3));
            }
    }
    else
    {
        Rng rng(1);
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                Pixel_t pixel;
                getPixel(image, x, y, pixel);
                COMPIMAGEPIXELS(pixel, toCT<CT>(rng()), toCT<CT>(rng()), toCT<CT>(rng()),
                                toCT<CT>(rng()));
            }
    }

    return true;
}

static bool testImages1()
{
    static const int sideSizes[] =
    {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 17, 20, 50, 100, 255, 256, 900
    };
    const int sideSizesAmount = sizeof(sideSizes)/sizeof(*sideSizes);

    WPngImage image;

#define ERRORRETIMAGECOMP do { \
        std::cout << " -> patternIndex=" << patternIndex << ", image size=(" << width \
                  << ", " << height << ").\n"; ERRORRET; } while(0)

    for(int patternIndex = 0; patternIndex < kPatternsAmount; ++patternIndex)
    {
        const int width = 100, height = 150;
        WPngImage smallerTestImage(50, 60, WPngImage::Pixel8(0, 1, 2, 3));
        setTestPattern<WPngImage::Pixel8>(patternIndex, image, width, height);
        smallerTestImage = image;
        if(!checkTestPattern<WPngImage::Pixel8>(patternIndex, smallerTestImage, width, height))
            ERRORRETIMAGECOMP;

        WPngImage largerTestImage(200, 260, WPngImage::Pixel8(0, 1, 2, 3));
        largerTestImage = image;
        if(!checkTestPattern<WPngImage::Pixel8>(patternIndex, largerTestImage, width, height))
            ERRORRETIMAGECOMP;

        for(int heightSizeIndex = 0; heightSizeIndex < sideSizesAmount; ++heightSizeIndex)
        {
            const int height = sideSizes[heightSizeIndex];
            for(int widthSizeIndex = 0; widthSizeIndex < sideSizesAmount; ++widthSizeIndex)
            {
                const int width = sideSizes[widthSizeIndex];

                setTestPattern<WPngImage::Pixel8>(patternIndex, image, width, height);
                if(!checkTestPattern<WPngImage::Pixel8>(patternIndex, image, width, height))
                    ERRORRETIMAGECOMP;

                WPngImage imageCopy(image);
                if(!checkTestPattern<WPngImage::Pixel8>(patternIndex, imageCopy, width, height))
                    ERRORRETIMAGECOMP;

                setTestPattern<WPngImage::Pixel16>(patternIndex, image, width, height);
                if(!checkTestPattern<WPngImage::Pixel16>(patternIndex, image, width, height))
                    ERRORRETIMAGECOMP;

                imageCopy = image;
                if(!checkTestPattern<WPngImage::Pixel16>(patternIndex, imageCopy, width, height))
                    ERRORRETIMAGECOMP;

                setTestPattern<WPngImage::PixelF>(patternIndex, image, width, height);
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, image, width, height))
                    ERRORRETIMAGECOMP;

                imageCopy = image;
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, imageCopy, width, height))
                    ERRORRETIMAGECOMP;

#if !WPNGIMAGE_RESTRICT_TO_CPP98
                WPngImage imageCopy2(std::move(image));
                if(image.width() > 0 || image.height() > 0)
                {
                    std::cout << "Move constructor did not work properly.\n"; ERRORRET;
                }
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, imageCopy2, width, height))
                    ERRORRETIMAGECOMP;

                imageCopy = std::move(imageCopy2);
                if(imageCopy2.width() > 0 || imageCopy2.height() > 0)
                {
                    std::cout << "Move assignment did not work properly.\n"; ERRORRET;
                }
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, imageCopy, width, height))
                    ERRORRETIMAGECOMP;
#else
                WPngImage imageCopy2;
#endif

                imageCopy2.move(imageCopy);
                if(imageCopy.width() > 0 || imageCopy.height() > 0)
                {
                    std::cout << "move() did not work properly.\n"; ERRORRET;
                }
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, imageCopy2, width, height))
                    ERRORRETIMAGECOMP;

                imageCopy.swap(imageCopy2);
                if(imageCopy2.width() > 0 || imageCopy2.height() > 0)
                {
                    std::cout << "swap() did not work properly.\n"; ERRORRET;
                }
                if(!checkTestPattern<WPngImage::PixelF>(patternIndex, imageCopy, width, height))
                    ERRORRETIMAGECOMP;
            }
        }
    }

    return true;
}

template<typename Pixel_t>
static bool testSingleColorImage(int width, int height, WPngImage::PixelFormat pixelFormat,
                                 Pixel_t(WPngImage::*getFunc)(int, int) const,
                                 Pixel_t setPixel, Pixel_t expectedPixel)
{
    WPngImage image(width, height, setPixel, pixelFormat);

    if(image.width() != width || image.height() != height)
    {
        std::cout << "Image dimensions are (" << image.width() << ", " << image.height()
                  << ") instead of (" << width << ", " << height << ")\n"; ERRORRET;
    }

    for(int y = 0; y < height; ++y)
        for(int x = 0; x < width; ++x)
            if(!compare(__LINE__, (image.*getFunc)(x, y), expectedPixel))
            {
                std::cout << " -> at image coordinates (" << x << ", " << y << ")\n";
                return false;
            }

    return true;
}

static bool testImages2()
{
    const WPngImage::PixelFormat formats[] =
    {
        WPngImage::kPixelFormat_GA8,
        WPngImage::kPixelFormat_GA16,
        WPngImage::kPixelFormat_GAF,
    };

    for(int i = 0; i < 3; ++i)
    {
        if(!testSingleColorImage(20, 20, formats[i], &WPngImage::get8,
                                 WPngImage::Pixel8(20, 120, 240, 210),
                                 WPngImage::Pixel8(123, 123, 123, 210))) ERRORRET;
        if(i > 0 && !testSingleColorImage(21, 21, formats[i], &WPngImage::get16,
                                          WPngImage::Pixel16(2000, 12000, 24000, 21000),
                                          WPngImage::Pixel16(12110, 12110, 12110, 21000))) ERRORRET;
        if(!testSingleColorImage(22, 22, formats[i], &WPngImage::getF,
                                 WPngImage::PixelF(0.25, 0.5, 0.75, 0.125),
                                 WPngImage::PixelF(0.486, 0.486, 0.486, 0.125))) ERRORRET;
    }

    const WPngImage::PixelFormat formats2[] =
    {
        WPngImage::kPixelFormat_RGBA8,
        WPngImage::kPixelFormat_RGBA16,
        WPngImage::kPixelFormat_RGBAF,
    };

    for(int i = 0; i < 3; ++i)
    {
        if(!testSingleColorImage(23, 23, formats2[i], &WPngImage::get8,
                                 WPngImage::Pixel8(12, 34, 56, 78),
                                 WPngImage::Pixel8(12, 34, 56, 78))) ERRORRET;
        if(i > 0 && !testSingleColorImage(24, 24, formats2[i], &WPngImage::get16,
                                          WPngImage::Pixel16(12, 34, 56, 78),
                                          WPngImage::Pixel16(12, 34, 56, 78))) ERRORRET;
        if(!testSingleColorImage(25, 25, formats2[i], &WPngImage::getF,
                                 WPngImage::PixelF(0.5019, 0.25098, 0.7607, 0.125),
                                 WPngImage::PixelF(0.5019, 0.25098, 0.7607, 0.125))) ERRORRET;
    }

    WPngImage image(1, 1, WPngImage::Pixel16(10000, 22000, 35000, 50000),
                    WPngImage::kPixelFormat_RGBA8);
    COMPARE(image.get16(0, 0), 10023, 21845, 34952, 50115);

    image.newImage(1, 1, WPngImage::PixelF(0.8, 0.7, 0.4, 0.2),
                   WPngImage::kPixelFormat_RGBA8);
    COMPARE(image.getF(0, 0), 0.8f, 0.702f, 0.4f, 0.2f);

    const struct { WPngImage::Pixel8 pixel; int g8, g16; Float gF; }
    testData[] =
    {
        { WPngImage::Pixel8(0, 0, 0), 0, 0, 0.0f },
        { WPngImage::Pixel8(0, 0, 1), 0, 19, 0.00029f },
        { WPngImage::Pixel8(5, 0, 1), 1, 292, 0.0044f },
        { WPngImage::Pixel8(255, 25, 2), 129, 33158, 0.5059f },
        { WPngImage::Pixel8(255, 255, 5), 247, 63413, 0.96762f },
        { WPngImage::Pixel8(255, 255, 255), 255, 65535, 1.0f },
    };

    image.newImage(1, 1, WPngImage::Pixel8(), WPngImage::kPixelFormat_GA16);
    for(unsigned i = 0; i < sizeof(testData)/sizeof(*testData); ++i)
    {
        image.set(0, 0, testData[i].pixel);
        COMPARE(image.get8(0, 0), testData[i].g8, testData[i].g8, testData[i].g8, 255);
        COMPARE(image.get16(0, 0), testData[i].g16, testData[i].g16, testData[i].g16, 65535);
        COMPARE(image.getF(0, 0), testData[i].gF, testData[i].gF, testData[i].gF, 1.0f);
    }

    image.newImage(20, 20, WPngImage::Pixel8(128, 167, 85, 98));
    for(int y = 0; y < 20; ++y)
        for(int x = 0; x < 20; ++x)
            COMPARE(image.get8(x, y), 128, 167, 85, 98);
    image.convertToPixelFormat(WPngImage::kPixelFormat_GA8);
    for(int y = 0; y < 20; ++y)
        for(int x = 0; x < 20; ++x)
            COMPARE(image.get8(x, y), 155, 155, 155, 98);

    return true;
}


//============================================================================
// Test WPngImage::putImage() and WPngImage::drawImage()
//============================================================================
static bool checkPutImage(const WPngImage& image, int destImageWidth, int destImageHeight,
                          int destX, int destY, int srcImageWidth, int srcImageHeight,
                          int srcX, int srcY, int srcWidth, int srcHeight, bool srcIsGrayscale)
{
    if(srcX < 0) { destX -= srcX; srcWidth += srcX; srcX = 0; }
    if(srcY < 0) { destY -= srcY; srcHeight += srcY; srcY = 0; }
    if(srcX + srcWidth > srcImageWidth) srcWidth = srcImageWidth - srcX;
    if(srcY + srcHeight > srcImageHeight) srcHeight = srcImageHeight - srcY;

    Rng rng(12);
    for(int y1 = 0; y1 < destImageHeight; ++y1)
        for(int x1 = 0; x1 < destImageWidth; ++x1)
        {
            const WPngImage::Pixel8 pixel = image.get8(x1, y1);
            const int r = Byte(rng()), g = Byte(rng()), b = Byte(rng()), a = Byte(rng());
            const int x2 = x1 - destX + srcX, y2 = y1 - destY + srcY;

            bool ok = true;
            if(x2 >= srcX && x2 < srcX + srcWidth &&
               y2 >= srcY && y2 < srcY + srcHeight)
            {
                if(image.isGrayscalePixelFormat() || srcIsGrayscale)
                {
                    const int c = WPngImage::Pixel8(x2, y2, 0, 255).toGrayCIE();
                    if(!compare(__LINE__, pixel, c, c, c, 255))
                        ok = false;
                }
                else if(!compare(__LINE__, pixel, int(Byte(x2)), int(Byte(y2)), 0, 255))
                    ok = false;
            }
            else
            {
                if(image.isGrayscalePixelFormat())
                {
                    const int c = WPngImage::Pixel8(r, g, b, a).toGrayCIE();
                    if(!compare(__LINE__, pixel, c, c, c, a))
                        ok = false;
                }
                else if(!compare(__LINE__, pixel, r, g, b, a))
                    ok = false;
            }
            if(!ok)
            {
                std::cout << "- with destImageWidth=" << destImageWidth
                          << ", destImageHeight=" << destImageHeight
                          << ", srcImageWidth=" << srcImageWidth
                          << ", srcImageHeight=" << srcImageHeight
                          << "\n  destX=" << destX << ", destY=" << destY
                          << ", srcX=" << srcX << ", srcY=" << srcY
                          << ", srcWidth=" << srcWidth << ", srcHeight=" << srcHeight
                          << ", x1=" << x1 << ", y1=" << y1
                          << ", x2=" << x2 << ", y2=" << y2 << "\n";
                return false;
            }
        }

    return true;
}

static bool testPutImage
(WPngImage::PixelFormat destPixelFormat, int destImageWidth, int destImageHeight,
 WPngImage::PixelFormat srcPixelFormat, int srcImageWidth, int srcImageHeight,
 int destX, int destY, int srcX, int srcY, int srcWidth, int srcHeight)
{
    Rng rng(12);
    WPngImage destImage(destImageWidth, destImageHeight, destPixelFormat);
    for(int y = 0; y < destImageHeight; ++y)
        for(int x = 0; x < destImageWidth; ++x)
        {
            const int r = Byte(rng()), g = Byte(rng()), b = Byte(rng()), a = Byte(rng());
            destImage.set(x, y, WPngImage::Pixel8(r, g, b, a));
        }

    WPngImage destImage2 = destImage;

    WPngImage srcImage(srcImageWidth, srcImageHeight, srcPixelFormat);
    for(int y = 0; y < srcImageHeight; ++y)
        for(int x = 0; x < srcImageWidth; ++x)
            srcImage.set(x, y, WPngImage::Pixel8(x, y, 0, 255));

    destImage.putImage(destX, destY, srcImage);
    if(!checkPutImage(destImage, destImageWidth, destImageHeight, destX, destY,
                      srcImageWidth, srcImageHeight, 0, 0, srcImageWidth, srcImageHeight,
                      srcImage.isGrayscalePixelFormat())) ERRORRET;

    destImage2.putImage(destX, destY, srcImage, srcX, srcY, srcWidth, srcHeight);
    if(!checkPutImage(destImage2, destImageWidth, destImageHeight, destX, destY,
                      srcImageWidth, srcImageHeight, srcX, srcY, srcWidth, srcHeight,
                      srcImage.isGrayscalePixelFormat())) ERRORRET;

    return true;
}

template<typename Pixel_t>
static bool testDrawImage(Pixel_t pixel1, Pixel_t pixel2, Pixel_t blended,
                          Pixel_t(WPngImage::*getFunc)(int, int) const)
{
    WPngImage image1(20, 20, pixel1), image2(10, 10, pixel2);
    image1.drawImage(5, 5, image2);

    for(int y = 0; y < 20; ++y)
    {
        for(int x = 0; x < 20; ++x)
        {
            if(x >= 5 && x < 15 && y >= 5 && y < 15)
            {
                COMPAREP((image1.*getFunc)(x, y), blended);
            }
            else
            {
                COMPAREP((image1.*getFunc)(x, y), pixel1);
            }
        }
    }
    return true;
}

template<typename Pixel_t>
static bool testDrawLine
(WPngImage::PixelFormat imagePixelFormat, int imageWidth, int imageHeight,
 WPngImage::Pixel8 imagePixel,
 int lineX, int lineY, int lineLength, bool horizontal, Pixel_t linePixel,
 WPngImage::Pixel8 bgPixel, WPngImage::Pixel8 resultPixel, bool useBlending)
{
    WPngImage image(imageWidth, imageHeight, imagePixel, imagePixelFormat);

    if(horizontal && useBlending)
        image.drawHorLine(lineX, lineY, lineLength, linePixel);
    else if(horizontal && !useBlending)
        image.putHorLine(lineX, lineY, lineLength, linePixel);
    else if(!horizontal && useBlending)
        image.drawVertLine(lineX, lineY, lineLength, linePixel);
    else
        image.putVertLine(lineX, lineY, lineLength, linePixel);

    if(lineLength < 0)
    {
        lineLength = -lineLength;
        if(horizontal) lineX = lineX - lineLength + 1;
        else lineY = lineY - lineLength + 1;
    }

    for(int y = 0; y < imageHeight; ++y)
        for(int x = 0; x < imageWidth; ++x)
        {
            const WPngImage::Pixel8 pixel = image.get8(x, y);
            if((horizontal && y == lineY && x >= lineX && x < lineX + lineLength) ||
               (!horizontal && x == lineX && y >= lineY && y < lineY + lineLength))
            {
                COMPAREP(pixel, resultPixel);
            }
            else
            {
                COMPAREP(pixel, bgPixel);
            }
        }

    // Test setting single pixel
    image.fill(imagePixel);

    if(useBlending)
        image.drawPixel(lineX, lineY, linePixel);
    else
        image.set(lineX, lineY, linePixel);

    for(int y = 0; y < imageHeight; ++y)
        for(int x = 0; x < imageWidth; ++x)
        {
            const WPngImage::Pixel8 pixel = image.get8(x, y);
            if(x == lineX && y == lineY)
            {
                COMPAREP(pixel, resultPixel);
            }
            else
            {
                COMPAREP(pixel, bgPixel);
            }
        }

    return true;
}

template<typename Pixel_t>
static bool testDrawRect
(WPngImage::PixelFormat imagePixelFormat, int imageWidth, int imageHeight,
 WPngImage::Pixel8 imagePixel,
 int rectX, int rectY, int rectWidth, int rectHeight, bool filled, Pixel_t rectPixel,
 WPngImage::Pixel8 bgPixel, WPngImage::Pixel8 resultPixel, bool useBlending)
{
    WPngImage image(imageWidth, imageHeight, imagePixel, imagePixelFormat);

    if(useBlending)
        image.drawRect(rectX, rectY, rectWidth, rectHeight, rectPixel, filled);
    else
        image.putRect(rectX, rectY, rectWidth, rectHeight, rectPixel, filled);

    if(rectWidth < 0) { rectWidth = -rectWidth; rectX = rectX - rectWidth + 1; }
    if(rectHeight < 0) { rectHeight = -rectHeight; rectY = rectY - rectHeight + 1; }

    const int maxX = rectX + rectWidth - 1, maxY = rectY + rectHeight - 1;

    for(int y = 0; y < imageHeight; ++y)
        for(int x = 0; x < imageWidth; ++x)
        {
            const WPngImage::Pixel8 pixel = image.get8(x, y);
            const bool ok = rectWidth != 0 && rectHeight != 0 &&
                ((filled && x >= rectX && x <= maxX && y >= rectY && y <= maxY) ||
                 (!filled && (((x == rectX || x == maxX) && y >= rectY && y <= maxY) ||
                              ((y == rectY || y == maxY) && x >= rectX && x <= maxX))))?
                compare(__LINE__, pixel, resultPixel) :
                compare(__LINE__, pixel, bgPixel);

            if(!ok)
            {
                std::cout << "- At x=" << x << ", y=" << y << "\n";
                return false;
            }
        }

    return true;
}

static bool testImages3()
{
    //------------------------------------------------------------------------
    // Test WPngImage::putImage()
    //------------------------------------------------------------------------
    const WPngImage::PixelFormat kPixelFormats[] =
    {
        WPngImage::kPixelFormat_RGBA8,
        WPngImage::kPixelFormat_RGBA16,
        WPngImage::kPixelFormat_RGBAF,
        WPngImage::kPixelFormat_GA8
    };
    const unsigned kFormatsAmount = sizeof(kPixelFormats)/sizeof(*kPixelFormats);

    const struct { int destImageWidth, destImageHeight, srcImageWidth, srcImageHeight,
            destX, destY, srcX, srcY, srcWidth, srcHeight; }
    kTestData[] =
    {
        { 20, 20, 10, 10, -4, -5, 2, 1, 8, 8 },
        { 20, 20, 10, 10, 0, -3, 3, 2, 6, 7 },
        { 20, 20, 10, 10, 15, -3, 1, 2, 8, 7 },
        { 20, 20, 10, 10, -5, 2, 2, 2, 6, 6 },
        { 20, 20, 10, 10, 3, 4, 3, 3, 5, 5 },
        { 20, 20, 10, 10, 15, 4, 3, 3, 4, 4 },
        { 20, 20, 10, 10, -2, 17, 1, 1, 8, 8 },
        { 20, 20, 10, 10, 4, 16, 0, 0, 8, 8 },
        { 20, 20, 10, 10, 16, 17, 1, 2, 7, 6 },
        { 20, 20, 20, 20, 0, 0, 5, 7, 10, 12 },
        { 25, 15, 10, 20, 8, -8, -5, -5, 20, 20 },
        { 22, 22, 30, 30, -3, -3, 10, 12, 12, 10 },
        { 20, 25, 5, 5, 5, 5, -6, -6, 20, 20 }
    };

    for(unsigned destFormatInd = 0; destFormatInd < kFormatsAmount; ++destFormatInd)
    {
        for(unsigned srcFormatInd = 0; srcFormatInd < kFormatsAmount; ++srcFormatInd)
        {
            for(unsigned dataInd = 0; dataInd < sizeof(kTestData)/sizeof(*kTestData); ++dataInd)
            {
                if(!testPutImage
                   (kPixelFormats[destFormatInd],
                    kTestData[dataInd].destImageWidth, kTestData[dataInd].destImageHeight,
                    kPixelFormats[srcFormatInd],
                    kTestData[dataInd].srcImageWidth, kTestData[dataInd].srcImageHeight,
                    kTestData[dataInd].destX, kTestData[dataInd].destY,
                    kTestData[dataInd].srcX, kTestData[dataInd].srcY,
                    kTestData[dataInd].srcWidth, kTestData[dataInd].srcHeight))
                {
                    std::cout << "- with destFormatInd=" << destFormatInd
                              << ", srcFormatInd=" << srcFormatInd
                              << ", dataInd=" << dataInd << "\n";
                    ERRORRET;
                }
            }
        }
    }


    //------------------------------------------------------------------------
    // Test WPngImage::drawImage()
    //------------------------------------------------------------------------
    if(!testDrawImage(WPngImage::Pixel8(62, 150, 200, 220),
                      WPngImage::Pixel8(177, 122, 21, 150),
                      WPngImage::Pixel8(133, 132, 88, 241), &WPngImage::get8)) ERRORRET;
    if(!testDrawImage(WPngImage::Pixel16(2062, 50150, 24200, 18220),
                      WPngImage::Pixel16(42177, 23122, 32321, 21150),
                      WPngImage::Pixel16(27395, 33080, 29328, 33490), &WPngImage::get16)) ERRORRET;
    if(!testDrawImage(WPngImage::PixelF(0.15, 0.6, 1.0, 0.9),
                      WPngImage::PixelF(0.8, 0.9, 0.25, 0.77),
                      WPngImage::PixelF(0.6623, 0.8364, 0.4089, 0.977), &WPngImage::getF)) ERRORRET;


    //------------------------------------------------------------------------
    // Test WPngImage::drawHorLine() and WPng::drawVertLine()
    //------------------------------------------------------------------------
    const struct {
        WPngImage::PixelFormat pixelFormat;
        WPngImage::Pixel8 imagePixel, linePixel, bgPixel, blendedPixel, assignedPixel; }
    kLineTestData[] =
    {
        {
            WPngImage::kPixelFormat_RGBA8,
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 255),
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 255),
            WPngImage::Pixel8(55, 66, 77, 255)
        },
        {
            WPngImage::kPixelFormat_RGBA8,
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 110),
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(88, 149, 49, 218),
            WPngImage::Pixel8(55, 66, 77, 110)
        },
        {
            WPngImage::kPixelFormat_RGBA16,
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(155, 166, 177, 100),
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(134, 205, 197, 255),
            WPngImage::Pixel8(155, 166, 177, 100)
        },
        {
            WPngImage::kPixelFormat_RGBAF,
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(155, 166, 177, 100),
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(134, 205, 197, 255),
            WPngImage::Pixel8(155, 166, 177, 100)
        },
        {
            WPngImage::kPixelFormat_GA8,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 224),
            WPngImage::Pixel8(165, 165, 165, 60)
        },
        {
            WPngImage::kPixelFormat_GA16,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 225),
            WPngImage::Pixel8(165, 165, 165, 60)
        },
        {
            WPngImage::kPixelFormat_GAF,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 224),
            WPngImage::Pixel8(165, 165, 165, 60)
        },
    };

    const int imageWidth = 8, imageHeight = 9, maxLineLength = 12;

    for(unsigned testIndex = 0;
        testIndex < sizeof(kLineTestData)/sizeof(*kLineTestData);
        ++testIndex)
    {
        for(int useBlending = 0; useBlending < 2; ++useBlending)
        {
            for(int startX = -2; startX < imageWidth + 1; ++startX)
            {
                for(int startY = -2; startY < imageHeight + 1; ++startY)
                {
                    for(int lineLength = -maxLineLength; lineLength <= maxLineLength; ++lineLength)
                    {
                        for(int horizontal = 0; horizontal < 2; ++horizontal)
                        {
                            if(!testDrawLine(kLineTestData[testIndex].pixelFormat,
                                             imageWidth, imageHeight,
                                             kLineTestData[testIndex].imagePixel,
                                             startX, startY, lineLength, horizontal,
                                             kLineTestData[testIndex].linePixel,
                                             kLineTestData[testIndex].bgPixel,
                                             useBlending ?
                                             kLineTestData[testIndex].blendedPixel :
                                             kLineTestData[testIndex].assignedPixel,
                                             useBlending))
                            {
                                std::cout << "- With testIndex=" << testIndex
                                          << ", lineLength=" << lineLength
                                          << ", startX=" << startX << ", startY=" << startY
                                          << ", horizontal=" << horizontal
                                          << ", useBlending=" << useBlending << "\n"
                                          << "  bgPixel=" << kLineTestData[testIndex].bgPixel
                                          << ", linePixel=" << kLineTestData[testIndex].linePixel
                                          << ", resultPixel="
                                          << (useBlending ?
                                              kLineTestData[testIndex].blendedPixel :
                                              kLineTestData[testIndex].assignedPixel) << "\n";
                                ERRORRET;
                            }
                        }
                    }

                    for(int height = -maxLineLength; height <= maxLineLength; ++height)
                    {
                        for(int width = -maxLineLength; width <= maxLineLength; ++width)
                        {
                            for(int filled = 0; filled < 2; ++filled)
                            {
                                if(!testDrawRect(kLineTestData[testIndex].pixelFormat,
                                                 imageWidth, imageHeight,
                                                 kLineTestData[testIndex].imagePixel,
                                                 startX, startY, width, height, filled,
                                                 kLineTestData[testIndex].linePixel,
                                                 kLineTestData[testIndex].bgPixel,
                                                 useBlending ?
                                                 kLineTestData[testIndex].blendedPixel :
                                                 kLineTestData[testIndex].assignedPixel,
                                                 useBlending))
                                {
                                    std::cout << "- With testIndex=" << testIndex
                                              << ", startX=" << startX << ", startY=" << startY
                                              << ", width=" << width << ", height=" << height
                                              << ", filled=" << filled
                                              << ", useBlending=" << useBlending << "\n"
                                              << "  bgPixel=" << kLineTestData[testIndex].bgPixel
                                              << ", linePixel="
                                              << kLineTestData[testIndex].linePixel
                                              << ", resultPixel="
                                              << (useBlending ?
                                                  kLineTestData[testIndex].blendedPixel :
                                                  kLineTestData[testIndex].assignedPixel) << "\n";
                                    ERRORRET;
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    //------------------------------------------------------------------------
    // Test WPngImage::resizeCanvas()
    //------------------------------------------------------------------------
    WPngImage image(10, 10);
    for(int y = 0; y < 10; ++y)
        for(int x = 0; x < 10; ++x)
            image.set(x, y, WPngImage::Pixel8(x, y, 0, 250));

    image.resizeCanvas(-5, -5, 20, 20);
    if(image.width() != 20 || image.height() != 20)
    {
        std::cout << "Image dimensions are (" << image.width() << ", " << image.height()
                  << ") instead of (20, 20)\n"; ERRORRET;
    }
    for(int y = 0; y < 20; ++y)
        for(int x = 0; x < 20; ++x)
        {
            const WPngImage::Pixel8 p = image.get8(x, y);
            if(y < 5 || y >= 15 || x < 5 || x >= 15)
            {
                COMPARE(p, 0, 0, 0, 0);
            }
            else
            {
                COMPARE(p, x-5, y-5, 0, 250);
            }
        }

    image.resizeCanvas(7, 7, 6, 6);
    if(image.width() != 6 || image.height() != 6)
    {
        std::cout << "Image dimensions are (" << image.width() << ", " << image.height()
                  << ") instead of (6, 6)\n"; ERRORRET;
    }
    for(int y = 0; y < 6; ++y)
        for(int x = 0; x < 6; ++x)
            COMPARE(image.get8(x, y), x+2, y+2, 0, 250);

    return true;
}


//============================================================================
// Test saving and loading against libpng (ie. that lodepng is used correctly)
//============================================================================
#ifdef TEST_AGAINST_LIBPNG
#include <png.h>

namespace
{
    struct PngPixelData
    {
        std::vector<WPngImage::Pixel16> pixels;
        int width, height;

        bool compareToImage(const WPngImage&) const;
    };

    struct PngStructs
    {
        png_structp mPngStructPtr;
        png_infop mPngInfoPtr;
        bool mReading;
        std::string mPngLibErrorMsg;

        PngStructs(bool);
        ~PngStructs();
        PngStructs(const PngStructs&);
        PngStructs& operator=(const PngStructs&);

        static void handlePngError(png_structp, png_const_charp);
        static void handlePngWarning(png_structp, png_const_charp);
    };

    struct FilePtr
    {
        std::FILE* fp;

        FilePtr(): fp(0) {}
        ~FilePtr() { if(fp) std::fclose(fp); }

     private:
        FilePtr(const FilePtr&);
        FilePtr& operator=(const FilePtr&);
    };
}

bool PngPixelData::compareToImage(const WPngImage& image) const
{
    if(image.width() != width || image.height() != height)
    {
        std::cout << "Dimensions do not match: "
                  << image.width() << "x" << image.height() << " vs. "
                  << width << "x" << height << "\n";
        ERRORRET;
    }

    for(int y = 0; y < height; ++y)
        for(int x = 0; x < width; ++x)
            if(!compare(__LINE__, image.get16(x, y), pixels[y*width + x]))
            {
                std::cout << "at pixel coordinates (" << x << ", " << y << ")\n";
                return false;
            }

    return true;
}

PngStructs::PngStructs(bool forReading):
    mPngStructPtr(0), mPngInfoPtr(0), mReading(forReading), mPngLibErrorMsg()
{
    if(forReading)
        mPngStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    else
        mPngStructPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if(mPngStructPtr) mPngInfoPtr = png_create_info_struct(mPngStructPtr);

    png_set_error_fn(mPngStructPtr, this, &handlePngError, &handlePngWarning);
}

PngStructs::~PngStructs()
{
    if(mReading)
    {
        if(mPngStructPtr && mPngInfoPtr) png_destroy_read_struct(&mPngStructPtr, &mPngInfoPtr, 0);
        else if(mPngStructPtr) png_destroy_read_struct(&mPngStructPtr, 0, 0);
    }
    else
    {
        if(mPngStructPtr && mPngInfoPtr) png_destroy_write_struct(&mPngStructPtr, &mPngInfoPtr);
        else if(mPngStructPtr) png_destroy_write_struct(&mPngStructPtr, 0);
    }
}

void PngStructs::handlePngError(png_structp png_ptr, png_const_charp errorMsg)
{
    PngStructs* obj = reinterpret_cast<PngStructs*>(png_get_error_ptr(png_ptr));
    obj->mPngLibErrorMsg = errorMsg;
    longjmp(png_jmpbuf(png_ptr), 1);
}

void PngStructs::handlePngWarning(png_structp, png_const_charp)
{}


//----------------------------------------------------------------------------
// Read PNG data
//----------------------------------------------------------------------------
namespace
{
    inline WPngImage::UInt16 getPNGComponent16
    (const std::vector<unsigned char>& rawImageData, std::size_t index)
    {
        return ((WPngImage::UInt16(rawImageData[index]) << 8) |
                WPngImage::UInt16(rawImageData[index + 1]));
    }

    inline void setPNGComponent16
    (std::vector<unsigned char>& rawImageData, std::size_t index, WPngImage::UInt16 value)
    {
        rawImageData[index] = (unsigned char)(value >> 8);
        rawImageData[index + 1] = (unsigned char)value;
    }
}

static void readPngData(PngStructs& structs, PngPixelData& dest)
{
    png_read_info(structs.mPngStructPtr, structs.mPngInfoPtr);

    const int imageWidth = png_get_image_width(structs.mPngStructPtr, structs.mPngInfoPtr);
    const int imageHeight = png_get_image_height(structs.mPngStructPtr, structs.mPngInfoPtr);
    const unsigned bitDepth = png_get_bit_depth(structs.mPngStructPtr, structs.mPngInfoPtr);

    png_set_add_alpha(structs.mPngStructPtr, 0xffff, PNG_FILLER_AFTER);
    png_set_palette_to_rgb(structs.mPngStructPtr);
    png_set_gray_to_rgb(structs.mPngStructPtr);

    png_read_update_info(structs.mPngStructPtr, structs.mPngInfoPtr);
    const unsigned rowBytes = png_get_rowbytes(structs.mPngStructPtr, structs.mPngInfoPtr);

    dest.width = imageWidth;
    dest.height = imageHeight;
    dest.pixels.clear();
    dest.pixels.reserve(imageWidth * imageHeight);

    if(bitDepth == 16)
    {
        assert(rowBytes <= 4*2*unsigned(imageWidth));
        std::vector<unsigned char> dataRow(4*2*imageWidth);

        for(int y = 0, destIndex = 0; y < imageHeight; ++y)
        {
            png_read_row(structs.mPngStructPtr, (png_bytep) &dataRow[0], 0);
            for(int x = 0; x < imageWidth*8; x += 8, ++destIndex)
                dest.pixels.push_back
                    (WPngImage::Pixel16(getPNGComponent16(dataRow, x),
                                        getPNGComponent16(dataRow, x + 2),
                                        getPNGComponent16(dataRow, x + 4),
                                        getPNGComponent16(dataRow, x + 6)));
        }
    }
    else
    {
        assert(rowBytes <= 4*unsigned(imageWidth));
        std::vector<Byte> dataRow(imageWidth * 4);

        for(int y = 0, destIndex = 0; y < imageHeight; ++y)
        {
            png_read_row(structs.mPngStructPtr, (png_bytep) &dataRow[0], 0);
            for(int x = 0; x < imageWidth*4; x += 4, ++destIndex)
                dest.pixels.push_back
                    (WPngImage::Pixel16
                     (WPngImage::Pixel8(dataRow[x], dataRow[x+1], dataRow[x+2], dataRow[x+3])));
        }
    }

    png_read_end(structs.mPngStructPtr, structs.mPngInfoPtr);
}


//----------------------------------------------------------------------------
// Load PNG image from file
//----------------------------------------------------------------------------
static bool loadPngDataFromFile(const char* fileName, PngPixelData& dest)
{
    FilePtr iFile;
    iFile.fp = std::fopen(fileName, "rb");
    if(!iFile.fp) { std::perror(fileName); ERRORRET; }

    png_byte header[8] = {};
    std::fread(header, 1, 8, iFile.fp);
    if(png_sig_cmp(header, 0, 8))
    {
        std::cout << fileName << ": PNG signature check failed\n";
        ERRORRET;
    }
    std::fseek(iFile.fp, 0, SEEK_SET);

    PngStructs structs(true);
    if(!structs.mPngInfoPtr)
    {
        std::cout << "Failed to initialize info_ptr\n";
        ERRORRET;
    }

    if(setjmp(png_jmpbuf(structs.mPngStructPtr)))
    {
        std::cout << fileName << ": libpng returned an error: \""
                  << structs.mPngLibErrorMsg << "\"\n";
        ERRORRET;
    }

    png_init_io(structs.mPngStructPtr, iFile.fp);
    readPngData(structs, dest);
    return true;
}


//----------------------------------------------------------------------------
// Read PNG data from RAM
//----------------------------------------------------------------------------
namespace
{
    struct RAMPngData
    {
        png_const_charp mData;
        std::size_t mDataSize, mCurrentDataIndex;
    };
}

static void pngDataReader(png_structp png_ptr, png_bytep data, png_size_t length)
{
    RAMPngData* obj = reinterpret_cast<RAMPngData*>(png_get_io_ptr(png_ptr));

    if(obj->mCurrentDataIndex + length > obj->mDataSize)
        png_error(png_ptr, "Read error");
    else
    {
        std::memcpy(data, obj->mData + obj->mCurrentDataIndex, length);
        obj->mCurrentDataIndex += length;
    }
}

static bool loadPngDataFromRAM
(const void* pngData, std::size_t pngDataSize, PngPixelData& dest)
{
    RAMPngData ramPngData;
    ramPngData.mData = (png_const_charp)pngData;
    ramPngData.mDataSize = pngDataSize;
    ramPngData.mCurrentDataIndex = 0;

    if(png_sig_cmp((png_bytep)ramPngData.mData, 0, 8))
    {
        std::cout << "PNG signature check failed\n";
        ERRORRET;
    }

    PngStructs structs(true);
    if(!structs.mPngInfoPtr)
    {
        std::cout << "Failed to initialize info_ptr\n";
        ERRORRET;
    }

    if(setjmp(png_jmpbuf(structs.mPngStructPtr)))
    {
        std::cout << "libpng returned an error: \""
                  << structs.mPngLibErrorMsg << "\"\n";
        ERRORRET;
    }

    png_set_read_fn(structs.mPngStructPtr, &ramPngData, &pngDataReader);
    readPngData(structs, dest);
    return true;
}

//----------------------------------------------------------------------------
// Write PNG data
//----------------------------------------------------------------------------
template<typename CT, typename Pixel_t>
static void setColorComponentsG(CT* dest, int colorComponents, const Pixel_t& pixel)
{
    dest[0] = pixel.toGrayCIE();
    if(colorComponents > 1) dest[1] = pixel.a;
}

template<typename CT, typename Pixel_t>
static void setColorComponents(CT* dest, int colorComponents, const Pixel_t& pixel)
{
    dest[0] = pixel.r;
    dest[1] = pixel.g;
    dest[2] = pixel.b;
    if(colorComponents > 3) dest[3] = pixel.a;
}

static void setPixelRow(const WPngImage& image, WPngImage::PngFileFormat fileFormat,
                        int y, Byte* dest, int colorComponents)
{
    switch(fileFormat)
    {
      case WPngImage::kPngFileFormat_GA8:
          for(int x = 0; x < image.width(); ++x)
              setColorComponentsG(dest + (x * colorComponents), colorComponents,
                                  image.get8(x, y));
          break;

      case WPngImage::kPngFileFormat_RGBA8:
          for(int x = 0; x < image.width(); ++x)
              setColorComponents(dest + (x * colorComponents), colorComponents,
                                 image.get8(x, y));
          break;

      default: break;
    }
}

static void setPixelRow(const WPngImage& image, WPngImage::PngFileFormat fileFormat,
                        int y, UInt16* dest, int colorComponents)
{
    switch(fileFormat)
    {
      case WPngImage::kPngFileFormat_GA16:
          for(int x = 0; x < image.width(); ++x)
              setColorComponentsG(dest + (x * colorComponents), colorComponents,
                                  image.get16(x, y));
          break;

      case WPngImage::kPngFileFormat_RGBA16:
          for(int x = 0; x < image.width(); ++x)
              setColorComponents(dest + (x * colorComponents), colorComponents,
                                 image.get16(x, y));
          break;

      default: break;
    }
}

static void performWritePngData
(const WPngImage& image, PngStructs& structs, WPngImage::PngFileFormat fileFormat,
 int bitDepth, int colorType, int colorComponents)
{
    const int imageWidth = image.width(), imageHeight = image.height();

    png_set_IHDR(structs.mPngStructPtr, structs.mPngInfoPtr, imageWidth, imageHeight,
                 bitDepth, colorType, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(structs.mPngStructPtr, structs.mPngInfoPtr);

    if(bitDepth == 16)
    {
        std::vector<UInt16> rowData(imageWidth * colorComponents);
        std::vector<unsigned char> rowDataBytes(rowData.size() * 2);
        for(int y = 0; y < imageHeight; ++y)
        {
            setPixelRow(image, fileFormat, y, &rowData[0], colorComponents);
            for(std::size_t i = 0; i < rowData.size(); ++i)
                setPNGComponent16(rowDataBytes, i * 2, rowData[i]);
            png_write_row(structs.mPngStructPtr, (png_bytep)(&rowDataBytes[0]));
        }
    }
    else
    {
        std::vector<Byte> rowData(imageWidth * colorComponents);
        for(int y = 0; y < imageHeight; ++y)
        {
            setPixelRow(image, fileFormat, y, &rowData[0], colorComponents);
            png_write_row(structs.mPngStructPtr, (png_bytep)(&rowData[0]));
        }
    }

    png_write_end(structs.mPngStructPtr, structs.mPngInfoPtr);
}

static void writePngData(const WPngImage& image, PngStructs& structs,
                         WPngImage::PngFileFormat fileFormat)
{
    const bool writeAlphas = !image.allPixelsHaveFullAlpha();

    switch(fileFormat)
    {
      case WPngImage::kPngFileFormat_GA8:
          performWritePngData
              (image, structs, fileFormat, 8,
               writeAlphas ? PNG_COLOR_TYPE_GRAY_ALPHA : PNG_COLOR_TYPE_GRAY,
               writeAlphas ? 2 : 1);
          break;

      case WPngImage::kPngFileFormat_GA16:
          performWritePngData
              (image, structs, fileFormat, 16,
               writeAlphas ? PNG_COLOR_TYPE_GRAY_ALPHA : PNG_COLOR_TYPE_GRAY,
               writeAlphas ? 2 : 1);
          break;

      case WPngImage::kPngFileFormat_none:
      case WPngImage::kPngFileFormat_RGBA8:
          performWritePngData
              (image, structs, fileFormat, 8,
               writeAlphas ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
               writeAlphas ? 4 : 3);
          break;

      case WPngImage::kPngFileFormat_RGBA16:
          performWritePngData
              (image, structs, fileFormat, 16,
               writeAlphas ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB,
               writeAlphas ? 4 : 3);
          break;
    }
}


//----------------------------------------------------------------------------
// Save PNG image to file
//----------------------------------------------------------------------------
bool savePngDataToFile(const WPngImage& image, const char* fileName,
                       WPngImage::PngFileFormat fileFormat)
{
    FilePtr oFile;
    oFile.fp = std::fopen(fileName, "wb");
    if(!oFile.fp) { std::perror(fileName); ERRORRET; }

    PngStructs structs(false);
    if(!structs.mPngInfoPtr)
    {
        std::cout << "Failed to initialize info_ptr\n";
        ERRORRET;
    }

    if(setjmp(png_jmpbuf(structs.mPngStructPtr)))
    {
        std::cout << "libpng returned an error: \""
                  << structs.mPngLibErrorMsg << "\"\n";
        ERRORRET;
    }

    png_init_io(structs.mPngStructPtr, oFile.fp);

    writePngData(image, structs, fileFormat);
    return true;
}


//----------------------------------------------------------------------------
// Write PNG data to RAM
//----------------------------------------------------------------------------
namespace
{
    struct PngDestData
    {
        std::vector<unsigned char>* destVector;
        WPngImage::ByteStreamOutputFunc destFunc;
        PngDestData(): destVector(0), destFunc(0) {}
        PngDestData(const PngDestData&);
        PngDestData& operator=(const PngDestData&);
    };
}

static void pngDataWriter(png_structp png_ptr, png_bytep data, png_size_t length)
{
    PngDestData* obj = reinterpret_cast<PngDestData*>(png_get_io_ptr(png_ptr));
    if(obj->destVector) obj->destVector->insert(obj->destVector->end(), data, data + length);
    if(obj->destFunc) obj->destFunc((const unsigned char*)data, length);
}

static void pngDataFlush(png_structp)
{}

static bool savePngDataToRAM
(const WPngImage& image,
 std::vector<unsigned char>* destVector, WPngImage::ByteStreamOutputFunc destFunc,
 WPngImage::PngFileFormat fileFormat)
{
    PngStructs structs(false);
    if(!structs.mPngInfoPtr)
    {
        std::cout << "Failed to initialize info_ptr\n";
        ERRORRET;
    }

    if(setjmp(png_jmpbuf(structs.mPngStructPtr)))
    {
        std::cout << "libpng returned an error: \""
                  << structs.mPngLibErrorMsg << "\"\n";
        ERRORRET;
    }

    PngDestData destData;
    destData.destVector = destVector;
    destData.destFunc = destFunc;

    png_set_write_fn(structs.mPngStructPtr, &destData, &pngDataWriter, &pngDataFlush);
    writePngData(image, structs, fileFormat);
    return true;
}

//----------------------------------------------------------------------------
// Check against libpng
//----------------------------------------------------------------------------
static bool loadImageFileWithLibpngAndCompare(const char* fileName, const WPngImage& image)
{
    PngPixelData pixelData;
    if(!loadPngDataFromFile(fileName, pixelData)) ERRORRET;
    if(!pixelData.compareToImage(image)) ERRORRET;
    return true;
}

static bool loadImageFromRAMWithLibpngAndCompare(const void* pngData, std::size_t pngDataSize,
                                                 const WPngImage& image)
{
    PngPixelData pixelData;
    if(!loadPngDataFromRAM(pngData, pngDataSize, pixelData)) ERRORRET;
    if(!pixelData.compareToImage(image)) ERRORRET;
    return true;
}
#endif


//============================================================================
// Test saving and loading
//============================================================================
static bool checkIOStatus(WPngImage::IOStatus status, bool saving)
{
    if(status == WPngImage::kIOStatus_Error_CantOpenFile)
    {
        const char* errorString = std::strerror(errno);
        std::cout << "Error trying to " << (saving ? "save to" : "open") << " PNG file: "
                  << errorString << "\n";
        ERRORRET;
    }
    else if(status != WPngImage::kIOStatus_Ok)
    {
        std::cout << (saving ? "Saving to" : "Loading")
                  << " PNG file failed with error code " << int(status) << ", message: \""
                  << status.pngLibErrorMsg << "\"\n";
        ERRORRET;
    }
    else if(!status.fileName.empty())
    {
        std::cout << (saving ? "Saving to" : "Loading")
                  << " PNG file was successful, but status.fileName is not empty: \""
                  << status.fileName << "\"\n";
        ERRORRET;
    }
    return true;
}

template<typename TestPixel_t, typename PixelData_t>
static bool checkRawPtr(const PixelData_t* ptr, int size,
                        typename TestPixel_t::Component_t maxValue)
{
    typedef typename TestPixel_t::Component_t CT;
    Rng rng(123);
    for(int i = 0; i < size; ++i)
    {
        const CT r = rng()*maxValue/65535, g = rng()*maxValue/65535,
            b = rng()*maxValue/65535, a = rng()*maxValue/65535;
        if(!compare(__LINE__, TestPixel_t(ptr[i]), r, g, b, a))
        {
            std::cout << "- at index " << i << "\n";
            return false;
        }
    }
    return true;
}

template<typename Pixel_t>
static bool checkLoadedImage(const WPngImage& image, WPngImage::PixelFormat pixelFormat,
                             WPngImage::PngFileFormat,
                             int width, int height, typename Pixel_t::Component_t maxValue,
                             Pixel_t(WPngImage::*getPixelFunc)(int, int) const)
{
    /* Note: This test works with libpng, but lodepng apparently optimizes images with
       all grayscale pixels to a grayscale PNG when saving, even when RGB(A) has been specified
       as the output format, thus causing this test to fail.
       Technically speaking this test is not necessary for the correctness of the library. */
    /*
    if(image.originalFileFormat() != fileFormat)
    {
        std::cout << "File format of loaded image (" << int(image.originalFileFormat())
                  << ") does not match that of the saved image (" << int(fileFormat) << ")\n";
        ERRORRET;
    }
    */

    if(image.currentPixelFormat() != pixelFormat)
    {
        std::cout << "Pixel format of loaded image (" << int(image.currentPixelFormat())
                  << ") does not match that of the saved image (" << int(pixelFormat) << ")\n";
        ERRORRET;
    }

    if(image.width() != width || image.height() != height)
    {
        std::cout << "Loaded image has dimensions (" << image.width() << ", " << image.height()
                  << ") instead of (" << width << ", " << height << ")\n";
        ERRORRET;
    }

    typedef typename Pixel_t::Component_t CT;

    Rng rng(123);

    if(image.isGrayscalePixelFormat())
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const CT g = rng()*maxValue/65535, a = rng()*maxValue/65535;
                if(!compare(__LINE__, (image.*getPixelFunc)(x, y), g, g, g, a))
                {
                    std::cout << "- at image coordinates (" << x << ", " << y << ")\n";
                    return false;
                }
            }
    }
    else
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const CT r = rng()*maxValue/65535, g = rng()*maxValue/65535,
                    b = rng()*maxValue/65535, a = rng()*maxValue/65535;
                if(!compare(__LINE__, (image.*getPixelFunc)(x, y), r, g, b, a))
                {
                    std::cout << "- at image coordinates (" << x << ", " << y << ")\n";
                    return false;
                }
            }

        const WPngImage::Pixel8* ptr8 = image.getRawPixelData8();
        const WPngImage::Pixel16* ptr16 = image.getRawPixelData16();
        const WPngImage::PixelF* ptrF = image.getRawPixelDataF();

        if(!((image.currentPixelFormat() == WPngImage::kPixelFormat_RGBA8 &&
              ptr8 && !ptr16 && !ptrF) ||
             (image.currentPixelFormat() == WPngImage::kPixelFormat_RGBA16 &&
              !ptr8 && ptr16 && !ptrF) ||
             (image.currentPixelFormat() == WPngImage::kPixelFormat_RGBAF &&
              !ptr8 && !ptr16 && ptrF)))
        {
            std::cout << "WPngImage::getRawPixelDataX() did not return a proper value "
                      << "(currentPixelFormat()=" << int(image.currentPixelFormat())
                      << ", ptr8=" << ptr8 << ", ptr16=" << ptr16 << ", ptrF=" << ptrF
                      << ")\n"; ERRORRET;
        }

        if(ptr8 && !checkRawPtr<Pixel_t>(ptr8, width*height, maxValue)) ERRORRET;
        if(ptr16 && !checkRawPtr<Pixel_t>(ptr16, width*height, maxValue)) ERRORRET;
        if(ptrF && !checkRawPtr<Pixel_t>(ptrF, width*height, maxValue)) ERRORRET;
    }

    return true;
}


template<typename Pixel_t>
static bool testSavingAndLoading(WPngImage::PixelFormat pixelFormat,
                                 WPngImage::PngFileFormat fileFormat,
                                 int width, int height,
                                 typename Pixel_t::Component_t maxValue,
                                 Pixel_t(WPngImage::*getPixelFunc)(int, int) const)
{
    typedef typename Pixel_t::Component_t CT;

    Rng rng(123);
    WPngImage image(width, height, pixelFormat);

    if(image.isGrayscalePixelFormat())
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const CT g = rng()*maxValue/65535, a = rng()*maxValue/65535;
                image.set(x, y, Pixel_t(g, g, g, a));
            }
    }
    else
    {
        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const CT r = rng()*maxValue/65535, g = rng()*maxValue/65535,
                    b = rng()*maxValue/65535, a = rng()*maxValue/65535;
                image.set(x, y, Pixel_t(r, g, b, a));
            }
    }

    if(!checkIOStatus(image.saveImage(kTestPngImageFileName, fileFormat), true)) ERRORRET;

    WPngImage image2;
    if(!checkIOStatus(image2.loadImage(kTestPngImageFileName, pixelFormat), false)) ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image2, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;

    std::FILE* iFile = std::fopen(kTestPngImageFileName, "rb");
    if(!iFile)
    {
        const char* errorString = std::strerror(errno);
        std::cout << "Error trying to open PNG file: " << errorString << "\n";
        ERRORRET;
    }
    std::fseek(iFile, 0, SEEK_END);
    const long fileSize = std::ftell(iFile);
    std::fseek(iFile, 0, SEEK_SET);
    std::vector<unsigned char> buffer(fileSize);
    std::fread(&buffer[0], 1, fileSize, iFile);
    std::fclose(iFile);

    WPngImage image3;
    if(!checkIOStatus(image3.loadImageFromRAM(&buffer[0], buffer.size(), pixelFormat), false))
        ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image3, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;

    std::vector<unsigned char> pngData;
    if(!checkIOStatus(image.saveImageToRAM(pngData, fileFormat), true)) ERRORRET;

    WPngImage image4;
    if(!checkIOStatus(image4.loadImageFromRAM(&pngData[0], pngData.size(), pixelFormat), false))
        ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image4, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;

#if !WPNGIMAGE_RESTRICT_TO_CPP98
    std::vector<unsigned char> pngData2;
    auto writeFunc = [&pngData2](const unsigned char* data, std::size_t length)
    { pngData2.insert(pngData2.end(), data, data + length); };
    if(!checkIOStatus(image.saveImageToRAM(writeFunc, fileFormat), true)) ERRORRET;

    WPngImage image5;
    if(!checkIOStatus(image5.loadImageFromRAM(&pngData2[0], pngData2.size(), pixelFormat), false))
        ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image5, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;
#endif

#ifdef TEST_AGAINST_LIBPNG
    if(!loadImageFileWithLibpngAndCompare(kTestPngImageFileName, image)) ERRORRET;
    if(!loadImageFromRAMWithLibpngAndCompare(&buffer[0], buffer.size(), image)) ERRORRET;

    std::remove(kTestPngImageFileName);
    if(!savePngDataToFile(image, kTestPngImageFileName, fileFormat)) ERRORRET;
    WPngImage image6;
    if(!checkIOStatus(image6.loadImage(kTestPngImageFileName, pixelFormat), false)) ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image6, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;

    buffer.clear();
    savePngDataToRAM(image, &buffer, 0, fileFormat);
    WPngImage image7;
    if(!checkIOStatus(image7.loadImageFromRAM(&buffer[0], buffer.size(), pixelFormat), false))
        ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image7, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;
#endif
    return true;
}

static bool testSavingAndLoading()
{
    if(!testSavingAndLoading<WPngImage::Pixel8>
       (WPngImage::kPixelFormat_RGBA8, WPngImage::kPngFileFormat_RGBA8,
        100, 120, 255, &WPngImage::get8)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel8>
       (WPngImage::kPixelFormat_GA8, WPngImage::kPngFileFormat_RGBA8,
        100, 110, 255, &WPngImage::get8)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel8>
       (WPngImage::kPixelFormat_GA8, WPngImage::kPngFileFormat_GA8,
        120, 110, 255, &WPngImage::get8)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel8>
       (WPngImage::kPixelFormat_RGBA8, WPngImage::kPngFileFormat_RGBA16,
        90, 120, 255, &WPngImage::get8)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel8>
       (WPngImage::kPixelFormat_RGBA16, WPngImage::kPngFileFormat_RGBA16,
        90, 122, 255, &WPngImage::get8)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel16>
       (WPngImage::kPixelFormat_RGBA16, WPngImage::kPngFileFormat_RGBA16,
        90, 124, 65535, &WPngImage::get16)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel16>
       (WPngImage::kPixelFormat_GA16, WPngImage::kPngFileFormat_RGBA16,
        90, 126, 65535, &WPngImage::get16)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::Pixel16>
       (WPngImage::kPixelFormat_GA16, WPngImage::kPngFileFormat_GA16,
        90, 127, 65535, &WPngImage::get16)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::PixelF>
       (WPngImage::kPixelFormat_GAF, WPngImage::kPngFileFormat_GA16,
        92, 127, 1.0f, &WPngImage::getF)) ERRORRET;

    if(!testSavingAndLoading<WPngImage::PixelF>
       (WPngImage::kPixelFormat_RGBAF, WPngImage::kPngFileFormat_RGBA16,
        94, 124, 1.0f, &WPngImage::getF)) ERRORRET;

    WPngImage image;
    const WPngImage::IOStatus status = image.loadImage("xyz");
    if(status != WPngImage::kIOStatus_Error_CantOpenFile)
    {
        std::cout << "Trying to open an inexistent file did not return proper status.\n";
        ERRORRET;
    }
    if(status.fileName != "xyz")
    {
        std::cout << "Trying to open an inexistent file did not assign the proper fileName value: "
            "\"" << status.fileName << "\" instead of \"xyz\"\n";
        ERRORRET;
    }

    return true;
}


//============================================================================
// Test the transform functions
//============================================================================
#if WPNGIMAGE_RESTRICT_TO_CPP98
WPngImage::Pixel8 invertFunc8(WPngImage::Pixel8 pixel) { return 255 - pixel; }
WPngImage::Pixel16 invertFunc16(WPngImage::Pixel16 pixel) { return 65535 - pixel; }
WPngImage::PixelF invertFuncF(WPngImage::PixelF pixel) { return 1.0f - pixel; }

WPngImage::TransformFunc8 getInvertFunc8() { return &invertFunc8; }
WPngImage::TransformFunc16 getInvertFunc16() { return &invertFunc16; }
WPngImage::TransformFuncF getInvertFuncF() { return &invertFuncF; }
#else
WPngImage::TransformFunc8 getInvertFunc8()
{ return [](WPngImage::Pixel8 pixel) { return 255 - pixel; }; }

WPngImage::TransformFunc16 getInvertFunc16()
{ return [](WPngImage::Pixel16 pixel) { return 65535 - pixel; }; }

WPngImage::TransformFuncF getInvertFuncF()
{ return [](WPngImage::PixelF pixel) { return 1.0f - pixel; }; }
#endif

template<typename Pixel_t>
static bool testTransformedImageContents(const WPngImage& destImage, const WPngImage& srcImage,
                                         typename Pixel_t::Component_t componentMaxValue)
{
    if(destImage.width() != srcImage.width() || destImage.height() != srcImage.height())
    {
        std::cout << "destImage is " << destImage.width() << "x" << destImage.height()
                  << ", but srcImage is " << srcImage.width() << "x" << srcImage.height()
                  << ".\n"; ERRORRET;
    }

    Pixel_t srcPixel, destPixel;

    for(int y = 0; y < destImage.height(); ++y)
    {
        for(int x = 0; x < destImage.width(); ++x)
        {
            getPixel(srcImage, x, y, srcPixel);
            getPixel(destImage, x, y, destPixel);
            const Pixel_t cmpPixel = componentMaxValue - srcPixel;
            COMPAREP(destPixel, cmpPixel);
        }
    }

    return true;
}

template<typename Pixel_t, typename TransformFunc_t>
static bool testTransformWithImage(const WPngImage& srcImage,
                                   typename Pixel_t::Component_t componentMaxValue,
                                   TransformFunc_t transformFunc)
{
    WPngImage destImage1, destImage2;

    srcImage.transform(transformFunc, destImage1);
    if(!testTransformedImageContents<Pixel_t>(destImage1, srcImage, componentMaxValue)) ERRORRET;

    destImage2 = srcImage;
    destImage2.transform(transformFunc);
    if(!testTransformedImageContents<Pixel_t>(destImage2, srcImage, componentMaxValue)) ERRORRET;

    return true;
}

template<typename Pixel_t, typename TransformFunc_t>
static bool testTransform(typename Pixel_t::Component_t componentMaxValue,
                          TransformFunc_t transformFunc)
{
    const int dimensions[] = { 1, 2, 6, 8, 50, 256 };
    const int kDimensionsAmount = sizeof(dimensions) / sizeof(*dimensions);

    for(int yDimInd = 0; yDimInd < kDimensionsAmount; ++yDimInd)
    {
        const int height = dimensions[yDimInd];
        for(int xDimInd = 0; xDimInd < kDimensionsAmount; ++xDimInd)
        {
            const int width = dimensions[xDimInd];
            WPngImage image(width, height, Pixel_t());

            for(int y = 0; y < height; ++y)
                for(int x = 0; x < width; ++x)
                    image.set(x, y, Pixel_t(x * componentMaxValue / width,
                                            y * componentMaxValue / height, 0));

            if(!testTransformWithImage<Pixel_t>(image, componentMaxValue, transformFunc)) ERRORRET;
        }
    }

    return true;
}

static bool testTransform()
{
    if(!testTransform<WPngImage::Pixel8>(255, getInvertFunc8())) ERRORRET;
    if(!testTransform<WPngImage::Pixel8>(255, getInvertFunc16())) ERRORRET;
    if(!testTransform<WPngImage::Pixel8>(255, getInvertFuncF())) ERRORRET;
    if(!testTransform<WPngImage::Pixel16>(65535, getInvertFunc16())) ERRORRET;
    if(!testTransform<WPngImage::Pixel16>(65535, getInvertFuncF())) ERRORRET;
    if(!testTransform<WPngImage::PixelF>(1.0f, getInvertFuncF())) ERRORRET;
    return true;
}


//============================================================================
// Test alpha premultiply
//============================================================================
template<typename Pixel_t, unsigned kAmount>
static bool testAlphaPremultiply(const Pixel_t (&pixels)[kAmount][2])
{
    for(unsigned i = 0; i < kAmount; ++i)
    {
        const Pixel_t p = pixels[i][0].premultipliedAlphaPixel();
        COMPAREP(p, pixels[i][1]);
        Pixel_t p2 = pixels[i][0];
        p2.premultiplyAlpha();
        COMPAREP(p2, pixels[i][1]);
    }

    WPngImage image(50, 60, Pixel_t(0));
    unsigned index = 0;
    for(int y = 0; y < image.height(); ++y)
        for(int x = 0; x < image.width(); ++x)
        {
            image.set(x, y, pixels[index][0]);
            if(++index == kAmount) index = 0;
        }

    image.premultiplyAlpha();

    for(int y = 0; y < image.height(); ++y)
        for(int x = 0; x < image.width(); ++x)
        {
            Pixel_t p;
            getPixel(image, x, y, p);
            COMPAREP(p, pixels[index][1]);
            if(++index == kAmount) index = 0;
        }

    return true;
}

static bool testAlphaPremultiply()
{
    const WPngImage::Pixel8 pixel8s[][2] =
    {
        { WPngImage::Pixel8(0, 0, 0, 255), WPngImage::Pixel8(0, 0, 0, 255) },
        { WPngImage::Pixel8(50, 60, 70, 255), WPngImage::Pixel8(50, 60, 70, 255) },
        { WPngImage::Pixel8(50, 60, 70, 64), WPngImage::Pixel8(13, 15, 18, 64) },
        { WPngImage::Pixel8(255, 128, 64, 50), WPngImage::Pixel8(50, 25, 13, 50) },
        { WPngImage::Pixel8(255, 128, 64, 0), WPngImage::Pixel8(0, 0, 0, 0) },
    };

    const WPngImage::Pixel16 pixel16s[][2] =
    {
        { WPngImage::Pixel16(0, 0, 0, 65535),
          WPngImage::Pixel16(0, 0, 0, 65535) },
        { WPngImage::Pixel16(5000, 6000, 7000, 65535),
          WPngImage::Pixel16(5000, 6000, 7000, 65535) },
        { WPngImage::Pixel16(5000, 6000, 7000, 6400),
          WPngImage::Pixel16(488, 586, 684, 6400) },
        { WPngImage::Pixel16(65535, 32768, 16384, 5000),
          WPngImage::Pixel16(5000, 2500, 1250, 5000) },
        { WPngImage::Pixel16(65535, 32768, 16384, 0),
          WPngImage::Pixel16(0, 0, 0, 0) },
    };

    const WPngImage::PixelF pixelFs[][2] =
    {
        { WPngImage::PixelF(0, 0, 0, 1), WPngImage::PixelF(0, 0, 0, 1) },
        { WPngImage::PixelF(0.2, 0.5, 0.7, 1), WPngImage::PixelF(0.2, 0.5, 0.7, 1) },
        { WPngImage::PixelF(1, 0.8, 0.5, 0.5), WPngImage::PixelF(0.5, 0.4, 0.25, 0.5) },
        { WPngImage::PixelF(1, 0.8, 0.5, 0), WPngImage::PixelF(0, 0, 0, 0) },
    };

    if(!testAlphaPremultiply(pixel8s)) ERRORRET;
    if(!testAlphaPremultiply(pixel16s)) ERRORRET;
    if(!testAlphaPremultiply(pixelFs)) ERRORRET;
    return true;
}


//============================================================================
// Test constexprness
//============================================================================
#if !WPNGIMAGE_RESTRICT_TO_CPP98
namespace
{
    constexpr WPngImage::Pixel8 kP8_1, kP8_2(9), kP8_3(8, 2), kP8_4(1, 2, 3), kP8_5(1, 2, 3, 4),
        kP8_6(0, 0, 0, 255);
    constexpr WPngImage::Pixel16 kP16_1, kP16_2(900), kP16_3(800, 2000), kP16_4(1000, 2000, 3000),
        kP16_5(1000, 2000, 3000, 4000), kP16_6(0, 0, 0, 65535);
    constexpr WPngImage::PixelF kPF_1, kPF_2(0.1), kPF_3(0.2, 0.3), kPF_4(0.1, 0.2, 0.3),
        kPF_5(0.1, 0.2, 0.3, 0.4), kPF_6(0, 0, 0, 1.0);
}

static_assert(kP8_1.r == 0 && kP8_1.g == 0 && kP8_1.b == 0 && kP8_1.a == 255, "Erroneous values");
static_assert(kP8_2.r == 9 && kP8_2.g == 9 && kP8_2.b == 9 && kP8_2.a == 255, "Erroneous values");
static_assert(kP8_3.r == 8 && kP8_3.g == 8 && kP8_3.b == 8 && kP8_3.a == 2, "Erroneous values");
static_assert(kP8_4.r == 1 && kP8_4.g == 2 && kP8_4.b == 3 && kP8_4.a == 255, "Erroneous values");
static_assert(kP8_5.r == 1 && kP8_5.g == 2 && kP8_5.b == 3 && kP8_5.a == 4, "Erroneous values");
static_assert(kP8_6.r == 0 && kP8_6.g == 0 && kP8_6.b == 0 && kP8_6.a == 255, "Erroneous values");
static_assert(kP8_1 == kP8_6 && kP8_2 != kP8_3 && kP8_4 != kP8_5, "Comparison failure");

static_assert(kP16_1.r == 0 && kP16_1.g == 0 && kP16_1.b == 0 && kP16_1.a == 65535,
              "Erroneous values");
static_assert(kP16_2.r == 900 && kP16_2.g == 900 && kP16_2.b == 900 && kP16_2.a == 65535,
              "Erroneous values");
static_assert(kP16_3.r == 800 && kP16_3.g == 800 && kP16_3.b == 800 && kP16_3.a == 2000,
              "Erroneous values");
static_assert(kP16_4.r == 1000 && kP16_4.g == 2000 && kP16_4.b == 3000 && kP16_4.a == 65535,
              "Erroneous values");
static_assert(kP16_5.r == 1000 && kP16_5.g == 2000 && kP16_5.b == 3000 && kP16_5.a == 4000,
              "Erroneous values");
static_assert(kP16_6.r == 0 && kP16_6.g == 0 && kP16_6.b == 0 && kP16_6.a == 65535,
              "Erroneous values");
static_assert(kP16_1 == kP16_6 && kP16_2 != kP16_3 && kP16_4 != kP16_5, "Comparison failure");

static_assert(kPF_1.r == 0.0f && kPF_1.g == 0.0f && kPF_1.b == 0.0f && kPF_1.a == 1.0f,
              "Erroneous values");
static_assert(kPF_2.r == 0.1f && kPF_2.g == 0.1f && kPF_2.b == 0.1f && kPF_2.a == 1.0f,
              "Erroneous values");
static_assert(kPF_3.r == 0.2f && kPF_3.g == 0.2f && kPF_3.b == 0.2f && kPF_3.a == 0.3f,
              "Erroneous values");
static_assert(kPF_4.r == 0.1f && kPF_4.g == 0.2f && kPF_4.b == 0.3f && kPF_4.a == 1.0f,
              "Erroneous values");
static_assert(kPF_5.r == 0.1f && kPF_5.g == 0.2f && kPF_5.b == 0.3f && kPF_5.a == 0.4f,
              "Erroneous values");
static_assert(kPF_6.r == 0.0f && kPF_6.g == 0.0f && kPF_6.b == 0.0f && kPF_6.a == 1.0f,
              "Erroneous values");
static_assert(kPF_1 == kPF_6 && kPF_2 != kPF_3 && kPF_4 != kPF_5, "Comparison failure");
#endif


//============================================================================
// main()
//============================================================================
int main()
{
    std::cout << "WPngImage v" WPNGIMAGE_VERSION_STRING ". Using libpng: "
              << (WPngImage::isUsingLibpng ? "yes" : "no")
              << ". Testing against libpng: "
#ifdef TEST_AGAINST_LIBPNG
              << "yes"
#else
              << "no"
#endif
              << ". Restrict to C++98: "
#if WPNGIMAGE_RESTRICT_TO_CPP98
              << "yes"
#else
              << "no"
#endif
              << "\n";

    if(!testPixelConstruction()) ERRORRET1;
    if(!testPixelOperators1<WPngImage::Pixel8>()) ERRORRET1;
    if(!testPixelOperators1<WPngImage::Pixel16>()) ERRORRET1;
    if(!testPixelOperators2()) ERRORRET1;
    if(!testColorSpaces()) ERRORRET1;
    if(!testImages1()) ERRORRET1;
    if(!testImages2()) ERRORRET1;
    if(!testImages3()) ERRORRET1;
    if(!testSavingAndLoading()) ERRORRET1;
    if(!testTransform()) ERRORRET1;
    if(!testAlphaPremultiply()) ERRORRET1;

    std::remove(kTestPngImageFileName);
    std::cout << "Ok.\n";
}
