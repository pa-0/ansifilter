/***************************************************************************
                     texgenerator.cpp  -  description
                             -------------------

    copyright            : (C) 2008 by Hans Meine
    email                : hans_meine@gmx.net
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

#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

#include "latexgenerator.h"
#include "version.h"

namespace ansifilter
{

LaTeXGenerator::LaTeXGenerator ():
    CodeGenerator(LATEX),
    fileSuffix(".tex")
{
    newLineTag="\\hspace*{\\fill}\\\\\n"; //avoid errors with empty input lines
    styleCommentOpen="/*";
    styleCommentClose="*/";
    spacer="\\ws{\\ }";
}

string LaTeXGenerator::getOpenTag()
{

    ostringstream fmtStream;


    if (elementStyle.isBold()) {
        fmtStream<< "\\bfseries{}";
    }
    if (elementStyle.isItalic()) {
        fmtStream<< "\\itshape{}";
    }

    if (elementStyle.isFgColorSet()) {
        fmtStream << "\\color[rgb]{"
                  << elementStyle.getFgColour().getRed(LATEX) << ","
                  << elementStyle.getFgColour().getGreen(LATEX) << ","
                  << elementStyle.getFgColour().getBlue(LATEX)
                  << "}";
    }

    return "{" + fmtStream.str();
}

string LaTeXGenerator::getCloseTag()
{
    return "}";
}

string LaTeXGenerator::getHyperlink(std::string_view uri, std::string_view txt){
    ostringstream os;
    os <<"\\href{"<<uri<<"}{"<<txt<<"}";
    return os.str();
}

string LaTeXGenerator::getGeneratorComment()
{
    ostringstream s;
    s <<"% LaTeX generated by ansifilter "
      << ANSIFILTER_VERSION << ", " <<  ANSIFILTER_URL <<"\n";

    return s.str();
}

string LaTeXGenerator::getHeader()
{
    ostringstream os;
    os << "\\documentclass{article}\n";
    os << "\\usepackage{color}\n";
    os << "\\usepackage{hyperref}\n";

    os << "\\newcommand{\\ws}[1]{\\textcolor[rgb]{0,0,0}{#1}}\n";
    if (encodingDefined()) {
        os << "%\\usepackage[" << encoding << "]{inputenc}\n";
    }
    if (!styleSheetPath.empty()) {
        os <<  "\\input {"<<styleSheetPath<<"}\n";
    }
    os << "\\begin{document}\n";
    os << "\\section*{" << docTitle << "}\n";
    os << "\\ttfamily%\n";
    return os.str();
}

string LaTeXGenerator::getFooter()
{
    string footer;
    footer = "\\end{document}\n";

    if (!omitVersionInfo)
        footer += getGeneratorComment();

    return footer;
}

void LaTeXGenerator::printBody()
{
    processInput();
}

string LaTeXGenerator::maskCharacter(unsigned char c)
{

    switch (c) {

    case ' ':
        return spacer;
        break;
    case '<' :
        return "$<$";
        break;
    case '>' :
        return "$>$";
        break;
    case '{':
    case '}':
    case '&':
    case '$':
    case '#':
    case '%': {
        string m( "\\" );
        m += c;
        return m;
    }
    break;
    // case '\"':
    //  return (fragmentOutput && replaceQuotes)?"\\dq{}":"\"";
    //   break;
    case '_':
        return "\\textunderscore ";
        break;
    case '^':
        return "\\textasciicircum ";
        break;
    case '\\':
        return "$\\backslash$";
        break;
    case '~':
        return "$\\sim$";
        break;
    case '|':
        return "\\textbar ";
        break;
    // avoid latex compilation failure if [ or * follows a line break (\\)
    case '*':
    case '[':
    case ']':
    // avoid "merging" of consecutive '-' chars when included in bold font ( \bf )
    case '-': {
        string m( 1, '{' );
        m += c;
        m += '}';
        return m;
    }
    break;

    case '\t' : // see deletion of nonprintable chars below
        return "\t";
        break;

    default :
        if (c>0x1f ) { // printable?
            return string( 1, c );;
        } else {
            return "";
        }
    }

}

void LaTeXGenerator::insertLineNumber ()
{
    if ( showLineNumbers ) {

        ostringstream lnum;
        lnum << setw ( 5 ) << right;
        if( numberCurrentLine ) {
            if (lineNumber>1)
              *out << getCloseTag();
            lnum << lineNumber;
            *out <<"{\\color[rgb]{0,0,0} "<<lnum.str()<<"}"<<spacer;
            *out << getOpenTag();
        } else {
            *out << lnum.str(); //for indentation
        }
    }
}

}
