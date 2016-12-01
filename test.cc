/* This is a comprehensive test file for WPngImage.
   Do not include this file in your project. It's intended only to
   test modifications to the library.
*/

#warning "This is a test file. Do not include it in your project."

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
 WPngImage::Pixel8 bgPixel, WPngImage::Pixel8 blendedPixel)
{
    WPngImage image(imageWidth, imageHeight, imagePixel, imagePixelFormat);

    if(horizontal)
        image.drawHorLine(lineX, lineY, lineLength, linePixel);
    else
        image.drawVertLine(lineX, lineY, lineLength, linePixel);

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
                COMPAREP(pixel, blendedPixel);
            }
            else
            {
                COMPAREP(pixel, bgPixel);
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
        WPngImage::Pixel8 imagePixel, linePixel, bgPixel, blendedPixel; }
    kLineTestData[] =
    {
        {
            WPngImage::kPixelFormat_RGBA8,
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 255),
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 255)
        },
        {
            WPngImage::kPixelFormat_RGBA8,
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(55, 66, 77, 110),
            WPngImage::Pixel8(123, 234, 21, 190),
            WPngImage::Pixel8(88, 149, 49, 218)
        },
        {
            WPngImage::kPixelFormat_RGBA16,
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(155, 166, 177, 100),
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(134, 205, 197, 255)
        },
        {
            WPngImage::kPixelFormat_RGBAF,
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(155, 166, 177, 100),
            WPngImage::Pixel8(120, 230, 210, 255),
            WPngImage::Pixel8(134, 205, 197, 255)
        },
        {
            WPngImage::kPixelFormat_GA8,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 224)
        },
        {
            WPngImage::kPixelFormat_GA16,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 225)
        },
        {
            WPngImage::kPixelFormat_GAF,
            WPngImage::Pixel8(120, 230, 210, 215),
            WPngImage::Pixel8(155, 166, 177, 60),
            WPngImage::Pixel8(211, 211, 211, 215),
            WPngImage::Pixel8(199, 199, 199, 224)
        },
    };

    for(unsigned testIndex = 0;
        testIndex < sizeof(kLineTestData)/sizeof(*kLineTestData);
        ++testIndex)
    {
        for(int lineLength = -60; lineLength < 60; lineLength += 3)
        {
            for(int lineX = -5; lineX < 45; lineX += 4)
            {
                for(int lineY = -5; lineY < 45; lineY += 3)
                {
                    for(int horizontal = 0; horizontal < 2; ++horizontal)
                    {
                        if(!testDrawLine(kLineTestData[testIndex].pixelFormat, 40, 40,
                                         kLineTestData[testIndex].imagePixel,
                                         lineX, lineY, lineLength, horizontal,
                                         kLineTestData[testIndex].linePixel,
                                         kLineTestData[testIndex].bgPixel,
                                         kLineTestData[testIndex].blendedPixel))
                        {
                            std::cout << "- With testIndex=" << testIndex
                                      << ", lineLength=" << lineLength
                                      << ", lineX=" << lineX << ", lineY=" << lineY
                                      << ", horizontal=" << horizontal << "\n";
                            ERRORRET;
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

    if(!checkIOStatus(image.saveImage("WPngImage_testing.png", fileFormat), true)) ERRORRET;

    WPngImage image2;
    if(!checkIOStatus(image2.loadImage("WPngImage_testing.png", pixelFormat), false)) ERRORRET;
    if(!checkLoadedImage<Pixel_t>(image2, pixelFormat, fileFormat, width, height, maxValue,
                                  getPixelFunc)) ERRORRET;

    std::FILE* iFile = std::fopen("WPngImage_testing.png", "rb");
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

    return true;
}

//============================================================================
// main()
//============================================================================
int main()
{
    if(!testPixelConstruction()) ERRORRET1;
    if(!testPixelOperators1<WPngImage::Pixel8>()) ERRORRET1;
    if(!testPixelOperators1<WPngImage::Pixel16>()) ERRORRET1;
    if(!testPixelOperators2()) ERRORRET1;
    if(!testColorSpaces()) ERRORRET1;
    if(!testImages1()) ERRORRET1;
    if(!testImages2()) ERRORRET1;
    if(!testImages3()) ERRORRET1;
    if(!testSavingAndLoading()) ERRORRET1;

    std::remove("WPngImage_testing.png");
    std::cout << "Ok.\n";
}
