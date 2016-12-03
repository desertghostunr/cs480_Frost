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

@File: OpenSimplexNoise.cpp

@Author: Andrew Frost (Ported Kurt Spencer's OpenSimplex Noise in Java to C++)

@Brief: C++ port of Kurt Spencer's OpenSimplex Noise in Java

@Note: only ported the 2D Noise portion

*********************************************/
//headers///////////////////////////////////
#include"OpenSimplexNoise.h"
#include<cmath>

//initialize static constants
const double OpenSimplexNoise::STRETCH_CONSTANT_2D = ( 1.0 / std::sqrt( 2.0 + 1.0 ) - 1.0 ) / 2.0;
const double OpenSimplexNoise::SQUISH_CONSTANT_2D = ( std::sqrt( 2.0 + 1.0 ) - 1.0 ) / 2.0;
const double OpenSimplexNoise::NORM_CONSTANT_2D = 47;

const char OpenSimplexNoise::gradients2D[ ] = { 5,  2,    2,  5,
                             -5,  2,   -2,  5, 
                              5, -2,    2, -5,
                             -5, -2,   -2, -5, };




OpenSimplexNoise::OpenSimplexNoise( long seed )
{
    std::vector<short> source;
    short index;
    int random;
    source.resize( 256 );
    perm.resize( 256 );
    for( index = 0; index < 256; index++ )
    {
        source[ index ] = index;
    }
        
    seed = seed * 6364136223846793005l + 1442695040888963407l;
    seed = seed * 6364136223846793005l + 1442695040888963407l;
    seed = seed * 6364136223846793005l + 1442695040888963407l;

    for( index = 255; index >= 0; index-- )
    {
        seed = seed * 6364136223846793005l + 1442695040888963407l;
        random = ( int ) ( ( seed + 31 ) % ( index + 1 ) );
        if( random < 0 )
        {
            random += ( index + 1 );
        }			
        perm[ index ] = source[ random ];
        source[ random ] = source[ index ];
    }
}

OpenSimplexNoise::OpenSimplexNoise( const OpenSimplexNoise & src ):perm( src.perm )
{

}

OpenSimplexNoise::~OpenSimplexNoise( )
{
    //nothing to do
}

double OpenSimplexNoise::getNoise( double x, double y )
{
    //Place input coordinates onto grid.
    double stretchOffset = ( x + y ) * STRETCH_CONSTANT_2D;
    double xs = x + stretchOffset;
    double ys = y + stretchOffset;

    //Floor to get grid coordinates of rhombus (stretched square) super-cell origin.
    int xsb = std::floor( xs );
    int ysb = std::floor( ys );

    //Skew out to get actual coordinates of rhombus origin. We'll need these later.
    double squishOffset = ( xsb + ysb ) * SQUISH_CONSTANT_2D;
    double xb = xsb + squishOffset;
    double yb = ysb + squishOffset;

    //Compute grid coordinates relative to rhombus origin.
    double xins = xs - xsb;
    double yins = ys - ysb;

    //Sum those together to get a value that determines which region we're in.
    double inSum = xins + yins;

    //Positions relative to origin point.
    double dx0 = x - xb;
    double dy0 = y - yb;

    //We'll be defining these inside the next block and using them afterwards.
    double dx_ext, dy_ext;
    int xsv_ext, ysv_ext;

    double value = 0;

    //Contribution (1,0)
    double dx1 = dx0 - 1 - SQUISH_CONSTANT_2D;
    double dy1 = dy0 - 0 - SQUISH_CONSTANT_2D;
    double attn1 = 2 - dx1 * dx1 - dy1 * dy1;
    if( attn1 > 0 )
    {
        attn1 *= attn1;
        value += attn1 * attn1 * extrapolate( xsb + 1, ysb + 0, dx1, dy1 );
    }

    //Contribution (0,1)
    double dx2 = dx0 - 0 - SQUISH_CONSTANT_2D;
    double dy2 = dy0 - 1 - SQUISH_CONSTANT_2D;
    double attn2 = 2 - dx2 * dx2 - dy2 * dy2;
    if( attn2 > 0 )
    {
        attn2 *= attn2;
        value += attn2 * attn2 * extrapolate( xsb + 0, ysb + 1, dx2, dy2 );
    }

    if( inSum <= 1 )
    { //We're inside the triangle (2-Simplex) at (0,0)
        double zins = 1 - inSum;
        if( zins > xins || zins > yins )
        { //(0,0) is one of the closest two triangular vertices
            if( xins > yins )
            {
                xsv_ext = xsb + 1;
                ysv_ext = ysb - 1;
                dx_ext = dx0 - 1;
                dy_ext = dy0 + 1;
            }
            else
            {
                xsv_ext = xsb - 1;
                ysv_ext = ysb + 1;
                dx_ext = dx0 + 1;
                dy_ext = dy0 - 1;
            }
        }
        else
        { //(1,0) and (0,1) are the closest two vertices.
            xsv_ext = xsb + 1;
            ysv_ext = ysb + 1;
            dx_ext = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
            dy_ext = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
        }
    }
    else
    { //We're inside the triangle (2-Simplex) at (1,1)
        double zins = 2 - inSum;
        if( zins < xins || zins < yins )
        { //(0,0) is one of the closest two triangular vertices
            if( xins > yins )
            {
                xsv_ext = xsb + 2;
                ysv_ext = ysb + 0;
                dx_ext = dx0 - 2 - 2 * SQUISH_CONSTANT_2D;
                dy_ext = dy0 + 0 - 2 * SQUISH_CONSTANT_2D;
            }
            else
            {
                xsv_ext = xsb + 0;
                ysv_ext = ysb + 2;
                dx_ext = dx0 + 0 - 2 * SQUISH_CONSTANT_2D;
                dy_ext = dy0 - 2 - 2 * SQUISH_CONSTANT_2D;
            }
        }
        else
        { //(1,0) and (0,1) are the closest two vertices.
            dx_ext = dx0;
            dy_ext = dy0;
            xsv_ext = xsb;
            ysv_ext = ysb;
        }
        xsb += 1;
        ysb += 1;
        dx0 = dx0 - 1 - 2 * SQUISH_CONSTANT_2D;
        dy0 = dy0 - 1 - 2 * SQUISH_CONSTANT_2D;
    }

    //Contribution (0,0) or (1,1)
    double attn0 = 2 - dx0 * dx0 - dy0 * dy0;
    if( attn0 > 0 )
    {
        attn0 *= attn0;
        value += attn0 * attn0 * extrapolate( xsb, ysb, dx0, dy0 );
    }

    //Extra Vertex
    double attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext;
    if( attn_ext > 0 )
    {
        attn_ext *= attn_ext;
        value += attn_ext * attn_ext * extrapolate( xsv_ext, ysv_ext, dx_ext, dy_ext );
    }

    return value / NORM_CONSTANT_2D;
}

const OpenSimplexNoise & OpenSimplexNoise::operator=( const OpenSimplexNoise & src )
{
    if( this != &src )
    {
        perm = src.perm;
    }

    return *this;
}

double OpenSimplexNoise::extrapolate( int xsb, int ysb, double dx, double dy )
{
    int index = perm[ ( perm[ xsb & 0xFF ] + ysb ) & 0xFF ] & 0x0E;

    if( index + 1 < 16 )
    {
        return gradients2D[ index ] * dx + gradients2D[ index + 1 ] * dy;
    }

    return 0;
    
}
