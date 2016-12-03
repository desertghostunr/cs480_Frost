//File Information ////////////////////////////
/*********************************************

@Original File: OpenSimplex Noise in Java

@Original Author: Kurt Spencer

@Reference File Address: https://gist.github.com/KdotJPG/b1270127455a94ac5d19

@Original License: 
		"This is free and unencumbered software released into the public domain.

		Anyone is free to copy, modify, publish, use, compile, sell, or
		distribute this software, either in source code form or as a compiled
		binary, for any purpose, commercial or non-commercial, and by any
		means.

		In jurisdictions that recognize copyright laws, the author or authors
		of this software dedicate any and all copyright interest in the
		software to the public domain. We make this dedication for the benefit
		of the public at large and to the detriment of our heirs and
		successors. We intend this dedication to be an overt act of
		relinquishment in perpetuity of all present and future rights to this
		software under copyright law.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
		EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
		MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
		IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
		OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
		ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
		OTHER DEALINGS IN THE SOFTWARE."

@File: OpenSimplexNoise.h

@Author: Andrew Frost (Ported Kurt Spencer's OpenSimplex Noise in Java to C++)

@Brief: C++ port of Kurt Spencer's OpenSimplex Noise in Java

@Note: only ported the 2D Noise portion

*********************************************/

//Pre-compiler directives ////////////////////
#ifndef OPENSIMPLEXNOISE_H
#define OPENSIMPLEXNOISE_H

//Header Files //////////////////////////////
#include<vector>

class OpenSimplexNoise
{
	public:
		static const double STRETCH_CONSTANT_2D;
		static const double SQUISH_CONSTANT_2D;
		static const double NORM_CONSTANT_2D;
		static const long DEFAULT_SEED = 0;

		static const char gradients2D[ ];

		static const short gradientSize = 16;
		
		OpenSimplexNoise( long seed = DEFAULT_SEED );
		OpenSimplexNoise( const OpenSimplexNoise& src );
		
		~OpenSimplexNoise( );

		double getNoise( double x, double y );

		const OpenSimplexNoise& operator=( const OpenSimplexNoise& src );

	private:

		std::vector<short> perm;

		double extrapolate( int xsb, int ysb, double dx, double dy );
		
};


#endif // !OPENSIMPLEXNOISE_H

