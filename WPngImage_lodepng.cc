/* WPngImage
   ---------
   This source code is released under the MIT license.
   For full documentation, consult the WPngImage.html file.
*/

#include "WPngImage.hh"
#include "lodepng.h"
#include <cstdio>
#include <algorithm>
#include <cassert>

//============================================================================
// Auxiliary functions for reading PNG data
//============================================================================
static WPngImage::PngFileFormat getFileFormat(unsigned bitDepth, unsigned colorType)
{
    if(bitDepth == 16)
        return (colorType == LCT_GREY || colorType == LCT_GREY_ALPHA ?
                WPngImage::kPngFileFormat_GA16 :
                WPngImage::kPngFileFormat_RGBA16);
    return (colorType == LCT_GREY || colorType == LCT_GREY_ALPHA ?
            WPngImage::kPngFileFormat_GA8 :
            WPngImage::kPngFileFormat_RGBA8);
}

namespace
{
    struct FilePtr
    {
        std::FILE* fp;

        FilePtr(): fp(0) {}
        ~FilePtr() { if(fp) std::fclose(fp); }

     private:
        FilePtr(const FilePtr&);
        FilePtr& operator=(const FilePtr&);
    };

    inline WPngImage::UInt16 getPNGComponent16
    (const std::vector<unsigned char>& rawImageData, std::size_t index)
    {
        return ((WPngImage::UInt16(rawImageData[index]) << 8) |
                WPngImage::UInt16(rawImageData[index + 1]));
    }
}


//============================================================================
// Read PNG data
//============================================================================
//----------------------------------------------------------------------------
// Load PNG image from file
//----------------------------------------------------------------------------
WPngImage::IOStatus WPngImage::performLoadImage
(const char* fileName, bool useConversion, PngReadConvert conversion, PixelFormat pixelFormat)
{
    FilePtr iFile;
    iFile.fp = std::fopen(fileName, "rb");
    if(!iFile.fp) return kIOStatus_Error_CantOpenFile;

    std::fseek(iFile.fp, 0, SEEK_END);
    const std::size_t fileSize = std::size_t(std::ftell(iFile.fp));
    std::fseek(iFile.fp, 0, SEEK_SET);

    std::vector<unsigned char> buffer(fileSize);
    std::fread(&buffer[0], 1, fileSize, iFile.fp);

    return performLoadImageFromRAM(&buffer[0], fileSize, useConversion, conversion, pixelFormat);
}


//----------------------------------------------------------------------------
// Read PNG data from RAM
//----------------------------------------------------------------------------
WPngImage::IOStatus WPngImage::performLoadImageFromRAM
(const void* pngData, std::size_t pngDataSize,
 bool useConversion, PngReadConvert conversion, PixelFormat pixelFormat)
{
    unsigned imageWidth = 0, imageHeight = 0;

    lodepng::State state;
    unsigned errorCode = lodepng_inspect
        (&imageWidth, &imageHeight, &state,
         reinterpret_cast<const unsigned char*>(pngData), pngDataSize);

    if(errorCode != 0) return kIOStatus_Error_NotPNG;

    const unsigned bitDepth = std::max(state.info_png.color.bitdepth, 8U);
    const unsigned colorType = state.info_png.color.colortype;
    const PngFileFormat fileFormat = ::getFileFormat(bitDepth, colorType);

    std::vector<unsigned char> rawImageData;
    errorCode = lodepng::decode(rawImageData, imageWidth, imageHeight,
                                reinterpret_cast<const unsigned char*>(pngData), pngDataSize,
                                LCT_RGBA, bitDepth);

    if(errorCode != 0)
        return IOStatus(kIOStatus_Error_PNGLibraryError, lodepng_error_text(errorCode));

    newImage(int(imageWidth), int(imageHeight),
             useConversion ? getPixelFormat(conversion, fileFormat) : pixelFormat);
    if(!mData) return kIOStatus_Ok;

    setFileFormat(fileFormat);

    if(bitDepth == 16)
    {
        assert(rawImageData.size() == imageWidth * imageHeight * 8);
        for(std::size_t srcIndex = 0, destIndex = 0;
            srcIndex < rawImageData.size();
            srcIndex += 8, ++destIndex)
            setPixel(destIndex,
                     Pixel16(getPNGComponent16(rawImageData, srcIndex),
                             getPNGComponent16(rawImageData, srcIndex + 2),
                             getPNGComponent16(rawImageData, srcIndex + 4),
                             getPNGComponent16(rawImageData, srcIndex + 6)));
    }
    else
    {
        assert(rawImageData.size() == imageWidth * imageHeight * 4);
        for(std::size_t srcIndex = 0, destIndex = 0;
            srcIndex < rawImageData.size();
            srcIndex += 4, ++destIndex)
            setPixel(destIndex,
                     Pixel8(rawImageData[srcIndex], rawImageData[srcIndex + 1],
                            rawImageData[srcIndex + 2], rawImageData[srcIndex + 3]));
    }

    return kIOStatus_Ok;
}


//============================================================================
// Write PNG data
//============================================================================
//----------------------------------------------------------------------------
// Save PNG image to file
//----------------------------------------------------------------------------
WPngImage::IOStatus WPngImage::saveImage(const char*, PngFileFormat) const
{
    return kIOStatus_Ok;
}


//----------------------------------------------------------------------------
// Write PNG data to RAM
//----------------------------------------------------------------------------
WPngImage::IOStatus WPngImage::performSaveImageToRAM
(std::vector<unsigned char>*, ByteStreamOutputFunc,
 PngFileFormat) const
{
    if(!mData) return kIOStatus_Ok;

    return kIOStatus_Ok;
}
