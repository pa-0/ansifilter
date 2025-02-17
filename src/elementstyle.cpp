/***************************************************************************
                          elementstyle.cpp  -  description
                             -------------------
    copyright            : (C) 2007-2024 by Andre Simon
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

#include "elementstyle.h"
#include <iostream>
namespace ansifilter
{

ElementStyle::ElementStyle()
    : bold(false), italic(false), underline(false), blink(false),
      reset(true), isNegativeMode(false), conceal(false), bgColorSet(false), fgColorSet(false),
      fgColID(0),
      bgColID(-1)
{}

ElementStyle::~ElementStyle() = default;

bool ElementStyle::isItalic() const
{
    return italic;
}
bool ElementStyle::isBlink() const
{
    return blink;
}
bool ElementStyle::isBold() const
{
    return bold;
}
bool ElementStyle::isUnderline() const
{
    return underline;
}
bool ElementStyle::isConceal() const
{
    return conceal;
}
bool ElementStyle::isBgColorSet() const
{
    return bgColorSet;
}
bool ElementStyle::isFgColorSet() const
{
    return fgColorSet;
}
const StyleColour ElementStyle::getFgColour() const
{
    return fgColour;
}
const StyleColour ElementStyle::getBgColour() const
{
    return bgColour;
}

void  ElementStyle::imageMode(bool negative)
{
    if (negative !=isNegativeMode) {
        StyleColour swapCol=getFgColour();
        setFgColour(getBgColour());
        setBgColour(swapCol);
        isNegativeMode=!isNegativeMode;
    }
}

void ElementStyle::setReset(bool b)
{
    reset=b;
    if (reset) {
      setFgColour("#000000");
      setFgColourID(0);
      setBgColourID(-1);
      bold = italic = underline = conceal = blink = bgColorSet = fgColorSet = false;
    }
}

}
