/***************************************************************************
                          rtfcode.h  -  description
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

#ifndef RTFGENERATOR_H
#define RTFGENERATOR_H

#include <string>
#include <map>
#include <string_view>
#include "codegenerator.h"


namespace ansifilter
{

/**
   \brief This class generates RTF.

   It contains information about the resulting document structure (document
   header and footer), the colour system, white space handling and text
   formatting attributes.

* @author Andre Simon
*/


struct PageSize {
    /// RTF page width
    int width;
    /// RTF page height
    int height;
    PageSize()
    {
    }
    /** Constructor to define page dimensions
        @param w width
        @param h height*/
    PageSize(int w, int h)
    {
        width=w;
        height = h;
    }
};

/** mapping of page size names and dimensions */
typedef  std::map<string, struct PageSize> PagesizeMap;

class RtfGenerator : public ansifilter::CodeGenerator
{
public:

    RtfGenerator();
    ~RtfGenerator();

    /** \param page size*/
    void setPageSize(const string & ps);

private:

    /** prints document header
     */
    string getHeader();

    /** Prints document footer*/
    string getFooter();

    /** Prints document body*/
    void printBody();

    /** Map of several pagesizes */
    PagesizeMap psMap;

    /** name of page size which is mapped to page dimensions*/
    string pageSize;


    /** flag to enable Unicode conversion */
    bool isUtf8;

    /** utf-8 to utf-16 conversion variables */
    unsigned long utf16Char;
    size_t utf8SeqLen;


    /** \return escaped character*/
    virtual string maskCharacter(unsigned char );

    /** \return escaped character*/
    virtual string maskCP437Character(unsigned char );

    virtual string getHyperlink(std::string_view uri, std::string_view txt);

    /**\return text formatting attributes in RTF format */
    string  getAttributes( const StyleColour & col);

    /** @param styleNumber number of current style
        @param elem associated element style
        @return RTF formatting sequence (colour index + bold + italic)*/
    string getOpenTag();

    /** @param elem associated element style
        @return RTF formatting sequence to close element formatting */
    string getCloseTag();


    void insertLineNumber ();
    string unicodeFromHTML(const string &);

};

}
#endif
