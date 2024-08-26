/***************************************************************************
                          stringtools.cpp  -  description
                             -------------------
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by Andre Simon
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

#include "stringtools.h"

// Avoid problems with isspace and UTF-8 characters, use iswspace
#include <cwctype>
#include <string_view>

namespace StringTools
{

string lowerCase(std::string_view s)
{
    char* buf = new char[s.length()];
    s.copy(buf, s.length());
    for(unsigned int i = 0; i < s.length(); i++)
        buf[i] = tolower(buf[i]);
    string r(buf, s.length());
    delete[] buf;
    return r;
}

// TODO only one function to change case
string upperCase(std::string_view s)
{
    char* buf = new char[s.length()];
    s.copy(buf, s.length());
    for(unsigned int i = 0; i < s.length(); i++)
        buf[i] = toupper(buf[i]);
    string r(buf, s.length());
    delete[] buf;
    return r;
}

vector<string> splitString(const string& s, unsigned char delim)
{
    string::size_type pos=s.find(delim), oldPos=0;
    vector <string> results;

    if (pos ==string::npos) {
        if (!s.empty())results.push_back(s);
        return results;
    }

    do {
        if (oldPos-pos) results.push_back (s.substr(oldPos, pos-oldPos));
        oldPos=pos+1;
        pos=s.find(delim, pos+1);
    } while (pos!=string::npos);
    results.push_back (s.substr(oldPos));

    return results;
}

}
