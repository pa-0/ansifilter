/***************************************************************************
                          cmdlineoptions.cpp  -  description
                             -------------------
    begin                : Sun Oct 13 2007
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

#include <cstring>
#include <string>
#include <vector>
#include <list>

#include "arg_parser.h"
#include "cmdlineoptions.h"
#include "platform_fs.h"
#include "stringtools.h"


const Arg_parser::Option options[] = {
    { 'a', "anchors",    Arg_parser::maybe },
    { 'd', "doc-title",  Arg_parser::yes },
    { 'e', "encoding",   Arg_parser::yes },
    { 'f', "fragment",   Arg_parser::no  },
    { 'F', "font",       Arg_parser::yes },
    { 'h', "help",       Arg_parser::no  },
    { 'H', "html",       Arg_parser::no  },
    { 'M', "pango",      Arg_parser::no  },
    { 'i', "input",      Arg_parser::yes },
    { 'l', "line-numbers", Arg_parser::no },
    { 'L', "latex",      Arg_parser::no  },
    { 'P', "tex",        Arg_parser::no  },
    { 'B', "bbcode",     Arg_parser::no  },
    { 'o', "output",     Arg_parser::yes },
    { 'O', "outdir",     Arg_parser::yes },
    { 'p', "plain",      Arg_parser::no  },
    { 'r', "style-ref",  Arg_parser::yes },
    { 'R', "rtf",        Arg_parser::no  },
    { 's', "font-size",  Arg_parser::yes },
    { 't', "tail",       Arg_parser::no  },
    { 'T', "text",       Arg_parser::no  },
    { 'w', "wrap",       Arg_parser::yes },
    { 'v', "version",    Arg_parser::no  },
    { 'V', "version",    Arg_parser::no  },
    { 'W', "wrap-no-numbers", Arg_parser::no  },
    { 'X', "art-cp437",  Arg_parser::no  },
    { 'U', "art-bin",    Arg_parser::no  },
    { 'D', "art-tundra", Arg_parser::no  },
    { 'Y', "art-width",  Arg_parser::yes  },
    { 'Z', "art-height", Arg_parser::yes  },
    { 'm', "map",        Arg_parser::yes },
    { 'N', "no-trailing-nl",  Arg_parser::no  },
    { 'C', "no-version-info", Arg_parser::no  },
    { 'k', "ignore-clear",    Arg_parser::maybe  },
    { 'c', "ignore-csi",      Arg_parser::no  },
    { 'y', "derived-styles",  Arg_parser::no  },
    { 'S', "svg",        Arg_parser::no  },
    { 'Q', "width",      Arg_parser::yes  },
    { 'E', "height",     Arg_parser::yes  },
    { 'x', "max-size",   Arg_parser::yes  },
    { 'g', "no-default-fg", Arg_parser::no  },
    { 'A', "line-append",   Arg_parser::yes  },

    {  0,  nullptr,           Arg_parser::no  }
};

CmdLineOptions::CmdLineOptions( const int argc, const char *argv[] ):
    outputType (ansifilter::TEXT),
    opt_help(false),
    opt_version(false),
    opt_fragment(false),
    opt_plain(false),
    opt_ignoreEOF(false),
    opt_linenum(false),
    opt_wrapNoNum(false),
    opt_anchors(false),
    opt_cp437(false),
    opt_asciiBin(false),
    opt_asciiTundra(false),
    opt_omit_trailing_cr(false),
    opt_omit_version_info(false),
    opt_ignoreClear(true),
    opt_ignoreCSI(false),
    opt_applyDynStyles(false),
    opt_genDynStyles(false),
    opt_funny_anchors(false),
    opt_omit_default_fg_color(false),
    encodingName("ISO-8859-1"),
    font("Courier New"),
    fontSize("10pt"),
    wrapLineLen(0),
    asciiArtWidth(80),
    asciiArtHeight(100),
    maxFileSize(268435456)
{
    char* hlEnvOptions=getenv("ANSIFILTER_OPTIONS");
    if (hlEnvOptions!=nullptr) {
        std::ostringstream envos;
        envos<<argv[0]<<" "<<hlEnvOptions;
        std::istringstream ss( envos.str());
        std::string arg;
        std::list<std::string> ls;
        std::vector<char*> options;
        while (ss >> arg)
        {
            ls.push_back(arg);
            options.push_back(const_cast<char*>(ls.back().c_str()));
        }
        options.push_back(nullptr);
        parseRuntimeOptions(options.size()-1, (const char**) &options[0], false);
    }

    parseRuntimeOptions(argc, argv);
}

CmdLineOptions::~CmdLineOptions() = default;

void CmdLineOptions::parseRuntimeOptions( const int argc, const char *argv[], bool readInputFilenames) {
        Arg_parser parser( argc, argv, options );
    if( parser.error().size() ) {			// bad option
        cerr << "ansifilter: "<< parser.error()<<"\n";
        cerr << "Try 'ansifilter --help' for more information.\n";
        exit( 1 );
    }

    int argind = 0;
    for( ; argind < parser.arguments(); ++argind ) {
        const int code = parser.code( argind );
        const std::string & arg = parser.argument( argind );
        if( !code ) break;
        switch( code ) {

        case 'a':
            opt_anchors = true;
            if ( arg=="self" ) opt_funny_anchors=true;
            break;
        case 'B':
            outputType = ansifilter::BBCODE;
            break;
        case 'd':
            docTitle = arg;
            break;
        case 'e':
            encodingName = arg;
            break;
        case 'f':
            opt_fragment = true;
            break;
        case 'F':
            font = arg;
            break;
        case 'h':
            opt_help = true;
            break;
        case 'H':
            outputType = ansifilter::HTML;
            break;
        case 'i':
            inputFileNames.push_back( arg );
            break;
        case 'l':
            opt_linenum=true;
            break;
        case 'L':
            outputType = ansifilter::LATEX;
            break;
        case 'm':
            colorMapPath = arg;
            break;
        case 'M':
            outputType = ansifilter::PANGO;
            break;
        case 'P':
            outputType = ansifilter::TEX;
            break;
        case 'o':
            outFilename = arg;
            break;
        case 'p':
            opt_plain = true;
            break;
        case 'r':
            styleSheetPath = arg;
            break;
        case 'R':
            outputType = ansifilter::RTF;
            break;
        case 'S':
            outputType = ansifilter::SVG;
            break;
        case 's':
            fontSize = arg;
            break;
        case 't':
            opt_ignoreEOF = true;
            break;
        case 'T':
            outputType = ansifilter::TEXT;
            break;
        case 'v':
        case 'V':
            opt_version = true;
            break;
        case 'O':
            outDirectory = validateDirPath( arg );
            break;
        case 'w':
            wrapLineLen=atoi(arg.c_str());
            break;
        case 'W':
            opt_wrapNoNum=true;
            break;
        case 'X':
            opt_cp437=true;
            break;
        case 'U':
            opt_asciiBin=true;
            break;
        case 'D':
            opt_asciiTundra=true;
            break;
        case 'Y':
            asciiArtWidth=atoi(arg.c_str());
            break;
        case 'Z':
            asciiArtHeight=atoi(arg.c_str());
            break;
        case 'N':
            opt_omit_trailing_cr=true;
            break;
        case 'C':
            opt_omit_version_info=true;
            break;
        case 'k':
            if (arg.size())
                opt_ignoreClear = ( arg=="true" || arg=="1" ) ;
            break;
        case 'c':
            opt_ignoreCSI = true;
            break;
        case 'y':
            opt_applyDynStyles=true;
            break;
        case 'g':
            opt_omit_default_fg_color = true;
            break;
        case 'Q':
            width=arg;
            break;
        case 'E':
            height=arg;
            break;
        case 'A':
            lineAppendage = arg;
            break;
        case 'x': {
            StringTools::str2num<off_t> ( maxFileSize, arg, std::dec );
            switch (arg[arg.size()-1]) {
                case 'G': maxFileSize *= 1024;
                case 'M': maxFileSize *= 1024;
                case 'K': maxFileSize *= 1024;
            }
            break;
        }
        default:
            cerr << "ansifilter: option parsing failed" << endl;
        }
    }

    if (readInputFilenames) {
        if (argind < parser.arguments()) { //still args left
            if  (inputFileNames.empty()) {
                while (argind < parser.arguments()) {
                    inputFileNames.push_back( parser.argument( argind++ ) );
                }
            }
        } else if (inputFileNames.empty()) {
            inputFileNames.emplace_back("");
        }
    }
}

string CmdLineOptions::validateDirPath(const string & path)
{
    return (path[path.length()-1] !=Platform::pathSeparator)?
           path+Platform::pathSeparator : path;
}

string CmdLineOptions::getSingleOutFilename()
{
    if (!inputFileNames.empty() && !outDirectory.empty()) {
        if (outFilename.empty()) {
            outFilename = outDirectory;
            int delim = getSingleInFilename().find_last_of(Platform::pathSeparator)+1;
            outFilename += getSingleInFilename().substr((delim>-1)?delim:0)
                           + getOutFileSuffix();
        }
    }
    return outFilename;
}

string CmdLineOptions::getSingleInFilename()  const
{
    return inputFileNames[0];
}

string CmdLineOptions::getOutDirectory()
{
    if (!outFilename.empty() && !inputFileNames.size()) {
        outDirectory=getDirName(outFilename);
    }
    return outDirectory;
}

string CmdLineOptions::getDirName(const string & path)
{
    size_t dirNameLength=path.rfind(Platform::pathSeparator);
    return (dirNameLength==string::npos)?string():path.substr(0, dirNameLength+1);
}

string CmdLineOptions::getLineAppendage() {
    return lineAppendage;
}


bool CmdLineOptions::printVersion()const
{
    return opt_version;
}

bool CmdLineOptions::printHelp()const
{
    return opt_help;
}

bool CmdLineOptions::fragmentOutput()const
{
    return opt_fragment;
}

bool CmdLineOptions::showLineNumbers()const
{
    return opt_linenum;
}

bool CmdLineOptions::parseCP437() const
{
  return opt_cp437;
}

bool CmdLineOptions::parseAsciiBin() const{
  return opt_asciiBin;
}

bool CmdLineOptions::parseAsciiTundra() const{
  return opt_asciiTundra;
}

bool CmdLineOptions::ignoreClearSeq() const {
    return opt_ignoreClear;
}

bool CmdLineOptions::ignoreCSISeq() const {
    return opt_ignoreCSI;
}

int CmdLineOptions::getAsciiArtWidth() const {
  return asciiArtWidth;
}
int CmdLineOptions::getAsciiArtHeight() const{
  return asciiArtHeight;
}

string CmdLineOptions::getOutFileSuffix()const
{
    switch (outputType) {
    case ansifilter::HTML:
        return ".html";
    case ansifilter::PANGO:
        return ".pango";
    case ansifilter::XHTML:
        return ".xhtml";
    case ansifilter::RTF:
        return ".rtf";
    case ansifilter::TEX:
    case ansifilter::LATEX:
        return ".tex";
    case ansifilter::BBCODE:
        return ".bbcode";
    case ansifilter::SVG:
        return ".svg";

    default:
        return ".txt";
    }
}

string CmdLineOptions::getEncoding() const
{
    return encodingName;
}

string CmdLineOptions::getFont() const
{
    return font;
}

string CmdLineOptions::getFontSize() const
{
    return fontSize;
}

string CmdLineOptions::getMapPath() const
{
  return colorMapPath;
}

bool CmdLineOptions::plainOutput() const
{
    return opt_plain;
}

bool CmdLineOptions::ignoreInputEOF() const
{
    return opt_ignoreEOF;
}

bool CmdLineOptions::wrapNoNumbers() const
{
    return opt_wrapNoNum;
}

bool CmdLineOptions::addAnchors() const
{
    return opt_anchors;
}
bool CmdLineOptions::addFunnyAnchors() const
{
    return opt_funny_anchors;
}

bool CmdLineOptions::omitEncoding() const
{
    return StringTools::lowerCase(encodingName)=="none";
}

bool CmdLineOptions::omitTrailingCR() const
{
    return opt_omit_trailing_cr;
}
bool CmdLineOptions::omitVersionInfo() const
{
    return opt_omit_version_info;
}
bool CmdLineOptions::applyDynStyles() const {
    return opt_applyDynStyles;
}
bool CmdLineOptions::omitDefaultForegroundColor() const
{
    return opt_omit_default_fg_color;
}

string CmdLineOptions::getDocumentTitle() const
{
    return docTitle;
}

string CmdLineOptions::getStyleSheetPath() const
{
    return styleSheetPath;
}

const vector <string> & CmdLineOptions::getInputFileNames() const
{
    return inputFileNames;
}

ansifilter::OutputType CmdLineOptions::getOutputType() const
{
    return outputType;
}

int CmdLineOptions::getWrapLineLength() const
{
    return wrapLineLen;
}

string CmdLineOptions::getWidth() const
{
    return width;
}

string CmdLineOptions::getHeight() const
{
    return height;
}

off_t CmdLineOptions::getMaxFileSize() const
{
    return maxFileSize;
}
