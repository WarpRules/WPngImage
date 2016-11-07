# WPngImage
WPngImage is a C++ wrapper class around the libpng library.

The main goal of WPngImage is to be as easy and simple to use as possible, while still being expressive and supporting a variety of PNG pixel formats. The design philosophy of this library is to aim for simplicity and ease-of-use, using a "plug&play" principle: Just one header file, and one source file. Simply add them to your project, and that's it. No myriads of source files, no configuration scripts and makefiles necessary. The only requirement is for libpng to be installed and available to your compiler.

WPngImage supports internal pixel representations in 8-bits-per-channel, 16-bits-per-channel and floating point, in RGBA or gray-alpha modes. The public interface of the class has been designed in such a manner that all these internal representations can be handled in the same way, with identical code, regardless of what the format of the pixels is (although, if necessary, the program can perform different operations depending on the internal pixel format.) A variety of arithmetic and colorspace conversion operations for pixels are provided.

WPngImage also supports decoding and encoding PNG images from/to memory (a feature that most other image manipulation libraries lack, even though this can be very useful in many situations.)

By default WPngImage uses C++11 for some convenience functionality and efficiency (eg. it implements a move constructor and assignment operator). The class can be compiled in C++98 mode if necessary, though.

Consult the WPngImage.html file for a tutorial and full reference documentation.
