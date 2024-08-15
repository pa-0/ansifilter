/***************************************************************************
                          rtfcode.cpp  -  description
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

#include <sstream>
#include <unordered_map>

#include "charcodes.h"
#include "version.h"
#include "rtfgenerator.h"
#include "stylecolour.h"

namespace ansifilter
{


RtfGenerator::RtfGenerator()
: CodeGenerator(RTF),
    pageSize("a4"), // Default: DIN A4
    isUtf8(false),
    utf16Char(0),
    utf8SeqLen(0)
{
    newLineTag = "\\line\n";
    spacer=" ";

    // Page dimensions
    psMap["a3"] = PageSize(16837,23811);
    psMap["a4"] = PageSize(11905,16837);
    psMap["a5"] = PageSize(8390,11905);

    psMap["b4"] = PageSize(14173,20012);
    psMap["b5"] = PageSize(9977,14173);
    psMap["b6"] = PageSize(7086,9977);

    psMap["letter"] = PageSize(12240,15840);
    psMap["legal"] = PageSize(12240,20163);
}

RtfGenerator::~RtfGenerator()
= default;

string RtfGenerator::getAttributes( const StyleColour & col)
{
    stringstream s;
    s  << "\\red"<< col.getRed(RTF)
       << "\\green"<<col.getGreen(RTF)
       << "\\blue"<<col.getBlue(RTF)
       << ";";
    return s.str();
}

string RtfGenerator::getOpenTag()
{
    ostringstream s;
    if (elementStyle.getFgColourID()>=0) {
        s << "{\\cf"<<(elementStyle.getFgColourID()+ 1);
    }
    if (elementStyle.getBgColourID()>=0) {
        s <<  "\\chcbpat"<<(elementStyle.getBgColourID()+1);
    }
    s <<"{";
    if (!parseCP437 &&  elementStyle.isBold()) s << "\\b ";
    if (elementStyle.isItalic()) s << "\\i ";
    if (elementStyle.isUnderline()) s << "\\ul ";
    return  s.str();
}

string  RtfGenerator::getCloseTag()
{
    ostringstream s;
    if (!parseCP437 && elementStyle.isBold()) s << "\\b0 ";
    if (elementStyle.isItalic()) s << "\\i0 ";
    if (elementStyle.isUnderline()) s << "\\ul0 ";
    s << "}}";
    return  s.str();
}

/* '{{\\field{\\*\\fldinst HYPERLINK "'..token..'" }{\\fldrslt\\ul\\ulc0 '..token..'}}}' */

string RtfGenerator::getHyperlink(string uri, string txt){
    ostringstream os;
    os <<R"({{\field{\*\fldinst HYPERLINK ")"<<uri<<R"(" }{\fldrslt\ul\ulc0 )"<<txt<<"}}}";
    return os.str();
}

string RtfGenerator::getHeader()
{
    return {};
}

void RtfGenerator::printBody()
{
    isUtf8 = encoding == "utf-8" || encoding == "UTF-8"; // FIXME

    *out << "{\\rtf1";

    if (parseCP437)
      *out<< "\\cpg437";
    else
      *out<< "\\ansi";

    *out <<" \\deff1"
         << R"({\fonttbl{\f1\fmodern\fprq1\fcharset0 )" ;
    *out << font ;
    *out << ";}}"
         << "{\\colortbl;";

    for (auto & i : workingPalette){
      *out << getAttributes(StyleColour(rgb2html(i)));
    }

    *out << "}\n";

    *out  << "\\paperw"<< psMap[pageSize].width <<"\\paperh"<< psMap[pageSize].height
          << R"(\margl1134\margr1134\margt1134\margb1134\sectd)" // page margins
          << R"(\plain\f1\fs)" ;  // Font formatting

    int fontSizeRTF=0;
    StringTools::str2num<int>(fontSizeRTF, fontSize, std::dec);
    *out << ((fontSizeRTF)? fontSizeRTF*2: 20);  // RTF needs double amount
    *out << "\n\\pard";

    //TODO save 24bit colors in RTF
    if (parseCP437/*||parseAsciiBin||parseAsciiTundra*/) *out << "\\cbpat1{";

    processInput();

    if (parseCP437/*||parseAsciiBin||parseAsciiTundra*/) *out << "}";

    *out << "}"<<std::endl;
}

string RtfGenerator::getFooter()
{
    return {};
}

string RtfGenerator::maskCharacter(unsigned char c)
{
  if (isUtf8 && c > 0x7f  && utf8SeqLen==0){

    //http://stackoverflow.com/questions/7153935/how-to-convert-utf-8-stdstring-to-utf-16-stdwstring

    if (c <= 0xDF)
    {
      utf16Char = c&0x1F;
      utf8SeqLen = 1;
    }
    else if (c <= 0xEF)
    {
      utf16Char = c&0x0F;
      utf8SeqLen = 2;
    }
    else if (c <= 0xF7)
    {
      utf16Char = c&0x07;
      utf8SeqLen = 3;
    } else {
      utf8SeqLen = 0;
    }
    return "";
  }

  if (utf8SeqLen) {
    utf16Char <<= 6;
    utf16Char += c & 0x3f;
    --utf8SeqLen;

    if (!utf8SeqLen){
      string m ( "\\u" );
      m += std::to_string(utf16Char);
      m += '?';
      utf16Char=0L;
      return m;
    } else {
      return "";
    }
  }

  switch (c) {
    case '}' :
    case '{' :
    case '\\' : {
        string m;
        m="\\";
        return m+=c;
    }
    break;
    case '\t' : // see deletion of nonprintable chars below
        return "\t";
        break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        string m;
        m="{";
        m+=c;
        m+="}";
        return m;
    }
    break;

    case AUML_LC:
        return "\\'e4";
        break;
    case OUML_LC:
        return "\\'f6";
        break;
    case UUML_LC:
        return "\\'fc";
        break;
    case AUML_UC:
        return "\\'c4";
        break;
    case OUML_UC:
        return "\\'d6";
        break;
    case UUML_UC:
        return "\\'dc";
        break;

    case AACUTE_LC:
        return "\\'e1";
        break;
    case EACUTE_LC:
        return "\\'e9";
        break;
    case OACUTE_LC:
        return "\\'f3";
        break;
    case UACUTE_LC:
        return "\\'fa";
        break;

    case AGRAVE_LC:
        return "\\'e0";
        break;
    case EGRAVE_LC:
        return "\\'e8";
        break;
    case OGRAVE_LC:
        return "\\'f2";
        break;
    case UGRAVE_LC:
        return "\\'f9";
        break;

    case AACUTE_UC:
        return "\\'c1";
        break;
    case EACUTE_UC:
        return "\\'c9";
        break;
    case OACUTE_UC:
        return "\\'d3";
        break;
    case UACUTE_UC:
        return "\\'da";
        break;
    case AGRAVE_UC:
        return "\\'c0";
        break;
    case EGRAVE_UC:
        return "\\'c8";
        break;
    case OGRAVE_UC:
        return "\\'d2";
        break;
    case UGRAVE_UC:
        return "\\'d9";
        break;

    case SZLIG:
        return "\\'df";
        break;

    default : {
        if (c>0x1f ) {
            return string( 1, c );
        } else {
            return "";
        }
    }
  }
}

string RtfGenerator::unicodeFromHTML(const string &htmlEntity){
  if (htmlEntity.length()!=8) return "";

  string decCode = "\\u", hexCode = htmlEntity.substr(3,4);

  int x=0;
  std::istringstream iss(hexCode);

  iss >> std::hex >> x;
  decCode += std::to_string(x);
  decCode +="?";

  return decCode;
}

std::string RtfGenerator::maskCP437Character(unsigned char c) {
    static const std::unordered_map<unsigned char, std::string> charMap = {
        {0x01, "&#x263a;"}, {0x02, "&#x263b;"}, {0x03, "&#x2665;"}, {0x04, "&#x2666;"},
        {0x05, "&#x2663;"}, {0x06, "&#x2660;"}, {0x08, "&#x25d8;"}, {0x0a, "&#x25d9;"},
        {0x0b, "&#x2642;"}, {0x0c, "&#x2640;"}, {0x10, "&#x25BA;"}, {0x11, "&#x25C4;"},
        {0x12, "&#x2195;"}, {0x13, "&#x203C;"}, {0x14, "&#x00b6;"}, {0x15, "&#x00a7;"},
        {0x16, "&#x25ac;"}, {0x17, "&#x21A8;"}, {0x18, "&#x2191;"}, {0x19, "&#x2193;"},
        {0x1a, "&#x2192;"}, {0x1b, "&#x2190;"}, {0x1c, "&#x221F;"}, {0x1d, "&#x2194;"},
        {0x1e, "&#x25B2;"}, {0x1f, "&#x25BC;"}, {0x80, "&#x00c7;"}, {0x81, "&#x00fc;"},
        {0x82, "&#x00e9;"}, {0x83, "&#x00e2;"}, {0x84, "&#x00e4;"}, {0x85, "&#x00e0;"},
        {0x86, "&#x00e5;"}, {0x87, "&#x00e7;"}, {0x88, "&#x00ea;"}, {0x89, "&#x00eb;"},
        {0x8a, "&#x00e8;"}, {0x8b, "&#x00ef;"}, {0x8c, "&#x00ee;"}, {0x8d, "&#x00ec;"},
        {0x8e, "&#x00c4;"}, {0x8f, "&#x00c5;"}, {0x90, "&#x00c9;"}, {0x91, "&#x00e6;"},
        {0x92, "&#x00c6;"}, {0x93, "&#x00f4;"}, {0x94, "&#x00f6;"}, {0x95, "&#x00f2;"},
        {0x96, "&#x00fb;"}, {0x97, "&#x00f9;"}, {0x98, "&#x00ff;"}, {0x99, "&#x00d6;"},
        {0x9a, "&#x00dc;"}, {0x9b, "&#x00a2;"}, {0x9c, "&#x00a3;"}, {0x9d, "&#x00a5;"},
        {0x9e, "&#x20a7;"}, {0x9f, "&#x0192;"}, {0xa0, "&#x00e1;"}, {0xa1, "&#x00ed;"},
        {0xa2, "&#x00f3;"}, {0xa3, "&#x00fa;"}, {0xa4, "&#x00f1;"}, {0xa5, "&#x00d1;"},
        {0xa6, "&#x00aa;"}, {0xa7, "&#x00ba;"}, {0xa8, "&#x00bf;"}, {0xa9, "&#x2310;"},
        {0xaa, "&#x00ac;"}, {0xab, "&#x00bd;"}, {0xac, "&#x00bc;"}, {0xad, "&#x00a1;"},
        {0xae, "&#x00ab;"}, {0xaf, "&#x00bb;"}, {0xe0, "&#x03b1;"}, {0xe1, "&#x00df;"},
        {0xe2, "&#x0393;"}, {0xe3, "&#x03c0;"}, {0xe4, "&#x03a3;"}, {0xe5, "&#x03c3;"},
        {0xe6, "&#x00b5;"}, {0xe7, "&#x03c4;"}, {0xe8, "&#x03a6;"}, {0xe9, "&#x0398;"},
        {0xea, "&#x03a9;"}, {0xeb, "&#x03b4;"}, {0xec, "&#x221e;"}, {0xed, "&#x03c6;"},
        {0xee, "&#x03b5;"}, {0xef, "&#x2229;"}, {0xf0, "&#x2261;"}, {0xf1, "&#x00b1;"},
        {0xf2, "&#x2265;"}, {0xf3, "&#x2264;"}, {0xf4, "&#x2320;"}, {0xf5, "&#x2321;"},
        {0xf6, "&#x00f7;"}, {0xf7, "&#x2248;"}, {0xf8, "&#x00b0;"}, {0xf9, "&#x2219;"},
        {0xfa, "&#x00b7;"}, {0xfb, "&#x221a;"}, {0xfc, "&#x207F;"}, {0xfd, "&#x00b2;"},
        {0xfe, "&#x25a0;"}, {0xff, "&#x00a0;"}
    };

    // Check for special cases first
    if (c == 0) {
        return " ";
    } else if (c == '}' || c == '{' || c == '\\') {
        return "\\" + std::string(1, c);
    } else if (std::isdigit(c)) {
        return "{" + std::string(1, c) + "}";
    } else if (c == '\t') {
        return "\t";
    }

    // Find in map
    auto it = charMap.find(c);
    if (it != charMap.end()) {
        return unicodeFromHTML(it->second);
    }

    // Default case for shades and box drawings
    static const std::unordered_map<unsigned char, std::string> specialMap = {
        {0xb0, "\\u9617?"}, {0xb1, "\\u9618?"}, {0xb2, "\\u9619?"},
        {0xb3, "\\u9474?"}, {0xb4, "\\u9508?"}, {0xb5, "\\u9569?"},
        {0xb6, "\\u9570?"}, {0xb7, "\\u9558?"}, {0xb8, "\\u9557?"},
        {0xb9, "\\u9571?"}, {0xba, "\\u9553?"}, {0xbb, "\\u9559?"},
        {0xbc, "\\u9565?"}, {0xbd, "\\u9564?"}, {0xbe, "\\u9563?"},
        {0xbf, "\\u9488?"}, {0xc0, "\\u9492?"}, {0xc1, "\\u9524?"},
        {0xc2, "\\u9516?"}, {0xc3, "\\u9500?"}, {0xc4, "\\u9472?"},
        {0xc5, "\\u9532?"}, {0xc6, "\\u9566?"}, {0xc7, "\\u9567?"},
        {0xc8, "\\u9562?"}, {0xc9, "\\u9556?"}, {0xca, "\\u9577?"},
        {0xcb, "\\u9574?"}, {0xcc, "\\u9568?"}, {0xcd, "\\u9552?"},
        {0xce, "\\u9580?"}, {0xcf, "\\u9575?"}, {0xd0, "\\u9576?"},
        {0xd1, "\\u9572?"}, {0xd2, "\\u9573?"}, {0xd3, "\\u9561?"},
        {0xd4, "\\u9560?"}, {0xd5, "\\u9554?"}, {0xd6, "\\u9555?"},
        {0xd7, "\\u9578?"}, {0xd8, "\\u9580?"}, {0xd9, "\\u9579?"},
        {0xda, "\\u9581?"}, {0xdb, "\\u9570?"}, {0xdc, "\\u9571?"},
        {0xdd, "\\u9565?"}, {0xde, "\\u9582?"}, {0xdf, "\\u9584?"},
        {0xe0, "\\u9516?"}, {0xe1, "\\u9508?"}, {0xe2, "\\u9510?"},
        {0xe3, "\\u9520?"}, {0xe4, "\\u9500?"}, {0xe5, "\\u9556?"},
        {0xe6, "\\u9566?"}, {0xe7, "\\u9563?"}, {0xe8, "\\u9562?"},
        {0xe9, "\\u9474?"}, {0xea, "\\u9552?"}, {0xeb, "\\u9532?"},
        {0xec, "\\u9516?"}, {0xed, "\\u9524?"}, {0xee, "\\u9576?"},
        {0xef, "\\u9573?"}, {0xf0, "\\u9560?"}, {0xf1, "\\u9512?"},
        {0xf2, "\\u9513?"}, {0xf3, "\\u9501?"}, {0xf4, "\\u9508?"},
        {0xf5, "\\u9514?"}, {0xf6, "\\u9500?"}, {0xf7, "\\u9517?"},
        {0xf8, "\\u9528?"}, {0xf9, "\\u9502?"}, {0xfa, "\\u9511?"},
        {0xfb, "\\u9508?"}, {0xfc, "\\u9523?"}, {0xfd, "\\u9518?"},
        {0xfe, "\\u9583?"}, {0xff, "\\u2588?"}
    };

    auto specialIt = specialMap.find(c);
    if (specialIt != specialMap.end()) {
        return specialIt->second;
    }

    return " "; // Default to a space for unknown characters
}


void RtfGenerator::setPageSize(const string & ps)
{
    if (psMap.count(ps)) pageSize = ps;
}

void RtfGenerator::insertLineNumber ()
{
    if ( showLineNumbers && !parseCP437 ) {

        ostringstream lnum;
        lnum << setw ( 5 ) << right;
        if( numberCurrentLine ) {
            lnum << lineNumber;
            *out <<lnum.str()<<spacer;
        } else {
            *out << lnum.str(); //for indentation
        }
    }
}

}
