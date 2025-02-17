/***************************************************************************
                     htmlgenerator.cpp  -  description
                             -------------------

    copyright            : (C) 2007-2023 by Andre Simon
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

#include "plaintextgenerator.h"
#include "version.h"

namespace ansifilter
{

PlaintextGenerator::PlaintextGenerator ():
    CodeGenerator(TEXT),
    fileSuffix(".txt")
{
    newLineTag="\n";
    styleCommentOpen="";
    styleCommentClose="";
    spacer=" ";
}


string PlaintextGenerator::getHeader()
{
    return "";
}

string PlaintextGenerator::getFooter()
{
    return "";
}

void PlaintextGenerator::printBody()
{
    processInput();
}

string PlaintextGenerator::maskCharacter(unsigned char c)
{
    if (c>0x1f || c=='\t') {
        return string( 1, c );;
    } else {
        return "";
    }
}

}
