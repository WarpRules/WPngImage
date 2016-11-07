# WPngImage
WPngImage is a C++ wrapper class around the libpng library.

The main goal of WPngImage is to be as easy and simple to use as possible, while still being expressive and supporting a variety of PNG pixel formats. The design philosophy of this library is to aim for simplicity and ease-of-use, using a "plug&play" principle: Just one header file, and one source file. Simply add them to your project, and that's it. No myriads of source files, no configuration scripts and makefiles necessary. The only requirement is for libpng to be installed and available to your compiler.

WPngImage supports internal pixel representations in 8-bits-per-channel, 16-bits-per-channel and floating point, in RGBA or gray-alpha modes. The public interface of the class has been designed in such a manner that all these internal representations can be handled in the same way, with identical code, regardless of what the format of the pixels is (although, if necessary, the program can perform different operations depending on the internal pixel format.) A variety of arithmetic and colorspace conversion operations for pixels are provided.

WPngImage also supports decoding and encoding PNG images from/to memory (a feature that most other image manipulation libraries lack, even though this can be very useful in many situations.)

By default WPngImage uses C++11 for some convenience functionality and efficiency (eg. it implements a move constructor and assignment operator). The class can be compiled in C++98 mode if necessary, though.

Consult the WPngImage.html file for a tutorial and full reference documentation.

## Simple examples

Create a 256x256 image with a red-green gradient, and save it to a PNG file:

```c++
#include "WPngImage.hh"

int main()
{
    WPngImage image(256, 256);

    for(int y = 0; y < image.height(); ++y)
        for(int x = 0; x < image.width(); ++x)
            image.set(x, y, WPngImage::Pixel8(x, y, 0));

    image.saveImage("example.png");
}
```

Load a PNG file, invert it (ie. calculate its negative), and save it to another file:

```c++
WPngImage image;
auto status = image.loadImage("photo.png");

if(status != WPngImage::kIOStatus_Ok)
{ /* handle error */ }
else
{
    for(int y = 0; y < image.height(); ++y)
        for(int x = 0; x < image.width(); ++x)
            image.set(x, y, 65535 - image.get16(x, y));

    image.saveImage("photo_negative.png");
}
```

(Note that in this example the original bit depth and color space (ie. rgb or gray) will be automatically preserved. These can be changed when loading or when saving, if needed.)
