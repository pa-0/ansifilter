/***************************************************************************
                          stylecolour.cpp  -  description
                             -------------------
    begin                : Die Nov 5 2002
    copyright            : (C) 2002 by Andre Simon
    email                : a.simon@mailbox.org
 ***************************************************************************/

/*
This file is part of ANSIFilter.

ANSIFilter is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ANSIFilter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ANSIFilter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stylecolour.h"
#include "stringtools.h"


#include <iostream>
#include <sstream>
#include <cmath>


namespace ansifilter
{

StyleColour::StyleColour(std::string_view red, std::string_view green, std::string_view blue)
{
    ostringstream rgbStream;
    rgbStream << red << " " << green << " " << blue;
    setRGB(rgbStream.str());
}

StyleColour::StyleColour()
{
    rgb.iRed = rgb.iGreen = rgb.iBlue = 0;
}

StyleColour::StyleColour(std::string_view styleColourString)
{
    setRGB(styleColourString);
}

void StyleColour::setRGB(std::string_view styleColourString)
{

    if (styleColourString.empty()) return;

    istringstream valueStream(styleColourString.data());
    string r, g, b;
    char c='\0';
    valueStream >> c;

    if (c=='#') {
        string htmlNotation;
        valueStream >> htmlNotation;
        if (htmlNotation.size() < 6) return;
        r = htmlNotation.substr(0, 2);
        g = htmlNotation.substr(2, 2);
        b = htmlNotation.substr(4, 2);
    } else {
        valueStream.putback(c);
        valueStream >> r;
        valueStream >> g;
        valueStream >> b;
    }

    StringTools::str2num<int>(rgb.iRed,   r, std::hex);
    StringTools::str2num<int>(rgb.iGreen, g, std::hex);
    StringTools::str2num<int>(rgb.iBlue,  b, std::hex);
}

void StyleColour::setRed(std::string_view red)
{
    StringTools::str2num<int>(rgb.iRed, red, std::hex);
}

void StyleColour::setGreen(std::string_view green)
{
    StringTools::str2num<int>(rgb.iGreen, green, std::hex);
}

void StyleColour::setBlue(std::string_view blue)
{
    StringTools::str2num<int>(rgb.iBlue, blue, std::hex);
}

const string StyleColour::getRed(OutputType type) const
{
    switch (type) {
    case RTF:
        return int2str(rgb.iRed, std::dec);
    case LATEX:
        return float2str( (float) rgb.iRed / 255);
    case TEX:
        return float2str( 1 - (float) rgb.iRed / 255);
    default:
        return int2str(rgb.iRed, std::hex);
    }
}

const string StyleColour::getGreen(OutputType type) const
{
    switch (type) {
    case RTF:
        return int2str(rgb.iGreen, std::dec);
    case LATEX:
        return float2str( (float) rgb.iGreen / 255);
    case TEX:
        return float2str( 1 - (float) rgb.iGreen / 255);
    default:
        return int2str(rgb.iGreen, std::hex);
    }
}

const string StyleColour::getBlue(OutputType type) const
{
    switch (type) {
    case RTF:
        return int2str(rgb.iBlue, std::dec);
    case LATEX:
        return float2str( (float) rgb.iBlue / 255);
    case TEX:
        return float2str( 1 - (float) rgb.iBlue / 255);
    default:
        return int2str(rgb.iBlue, std::hex);
    }
}


string StyleColour::int2str(const int num, std::ios_base& (*f)(std::ios_base&)) const
{
    std::ostringstream outStream;
    outStream.width(2);
    outStream.fill('0');
    outStream << f << num;

    return outStream.str();
}

string StyleColour::float2str(const double num) const
{
    std::ostringstream outStream;
    outStream << ( floor ( num * 100 + .5 ) / 100);

    return outStream.str();
}

}
