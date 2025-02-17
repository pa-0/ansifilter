/***************************************************************************
                          codegenerator.h  -  description
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

#ifndef CODEPARSER_H
#define CODEPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <iomanip>

// Avoid problems with isspace and UTF-8 characters, use iswspace instead
//#include <cctype>
#include <wctype.h>

#include "elementstyle.h"

#include "enums.h"
#include "stringtools.h"


using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::istream;
using std::ostream;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ofstream;
using std::ifstream;
using std::ios;
using std::right;
using std::setw;
using std::ios;


/// The ansifilter namespace contains all classes and data structures needed for parsing input data.

namespace ansifilter
{

  /** TheDraw output information of individual characters*/
  struct TDChar {
    unsigned char c;
    ElementStyle style;
  };


  class StyleInfo
{
public:

    /// Constructor
    StyleInfo() :  fgColor ( "" ), bgColor ( "" ), isBold (false), isItalic (false), isConcealed (false), isBlink (false), isUnderLine (false)
    {
    }

    /// Constructor
    StyleInfo (  const string& fgc, const string& bgc, bool b, bool i, bool c, bool blink, bool ul ) :
        fgColor ( fgc ), bgColor ( bgc ),isBold ( b ), isItalic ( i ), isConcealed ( c ), isBlink (blink), isUnderLine (ul)
    {
    }

    /// Copy Constructor
    StyleInfo ( const StyleInfo& other )
    {
        fgColor = other.fgColor;
        bgColor = other.bgColor;

        isItalic = other.isItalic;
        isBold = other.isBold;
        isConcealed=other.isConcealed;
        isBlink = other.isBlink;
        isUnderLine = other.isUnderLine;
    }

    /// Operator overloading
    StyleInfo& operator= ( const StyleInfo & other )
    {
        fgColor = other.fgColor;
        bgColor = other.bgColor;

        isItalic = other.isItalic;
        isBold = other.isBold;
        isConcealed=other.isConcealed;
        isBlink = other.isBlink;
        isUnderLine = other.isUnderLine;

        return *this;
    }

    bool operator==(const StyleInfo& r)
    {
        return this->fgColor==r.fgColor && this->bgColor==r.bgColor && this->isBold==r.isBold
                && this->isItalic==r.isItalic && this->isConcealed==r.isConcealed
                && this->isBlink==r.isBlink && this->isUnderLine==r.isUnderLine;
    }

    ~StyleInfo()
    {
    }

    string fgColor;    ///< foreground color
    string bgColor;    ///< background color
    bool isBold, isItalic, isConcealed, isBlink, isUnderLine;  ///< style properties
   };

/** \brief Base class for escape sequence parsing.

    The virtual class provides escape sequence parsing functionality.<br>
    The derived classes have to define the output format.<br>
    Codegenerator is a singleton class.

* @author Andre Simon
*/

class CodeGenerator
{

public:

    virtual ~CodeGenerator();

    /**
      Get appropriate Codegenerator instance
      \param type Output file type (TEXT, PANGO, HTML, RTF, LATEX, TEX)
    */
    static CodeGenerator* getInstance(OutputType type);

    /**
      Delete Codegenerator instance (this is intended for SWIG integration,
      in normal C++ code the result should be saved in a unique_ptr)
      \param CodeGenerator* CodeGenerator instance
    */
    static void deleteInstance(CodeGenerator* inst)
    {
        if (inst) delete inst;
    }

    /**
     Generates output file
     \param inFileName Path of input file (if empty use stdin)
     \param outFileName Path of output file (if empty use stdout)
     \return ParseError
    */
    ParseError generateFile(const string &inFileName, const string &outFileName);

    /**
     Generates output string from input string
     \param input input code string
     \return formatted output code
    */
    string generateString(const string &input);

    /**
     Generates output string from input file
     \param inFileName file path
     \return formatted output code
    */
    string generateStringFromFile(const string &inFileName);

    /**
     Generates output file from input string
     \param sourceStr input string
     \param outFileName output file path
     \param title document title
     \return formatted output code
    */
    ParseError generateFileFromString (const string &sourceStr,
                                       const string &outFileName,
                                       const string &title);

    /** Generate a stylesheet with the styles found in the document
    \param outPath Output path
    \return true if successful
     */
    virtual bool printDynamicStyleFile ( const string &outPath );

    /**
     Overrides default colours by user defined values; resets palette to default if mapPath is empty
     \param mapPath path of map file
     \return true if parsing was successful
    */
    bool setColorMap(const string& mapPath);

    /**
     Initialize default foreground color
    */
    void setDefaultForegroundColor();

    /** tell parser to omit document header and footer
       \param  flag true if output should be fragmented
    */
    void setFragmentCode(bool flag);

    /** \return fragment flag */
    bool getFragmentCode();

    /** tell parser to use this font as base font
    \param s the font name, e.g. "Courier New"
     */
    void setFont(const string& s);

    /** \return base font */
    const string getFont() const ;

    /** tell parser to use this size as base font size
    \param s the font size, e.g. "12"
     */
    void setFontSize(const string& s);

    /** \return base font size*/
    const string getFontSize();

    /** Set encloding
      \param encodingName encoding name
    */
    void setEncoding(const string& encodingName);

    /** \param title Document title */
    void setTitle(const string & title);

    /** \return Document title */
    string getTitle();

    /** \param path style sheet path*/
    void setStyleSheet(const string & path);

    /** \param b set to true if text formatting should be stripped */
    void setPlainOutput(bool b)
    {
        ignoreFormatting=b;
    }

    /** \param b set to true if HTML anchors should be added to line numbers */
    void setAddAnchors(bool b, bool self=false)
    {
        addAnchors=b;
        addFunnyAnchors=self;
    }

    /** \param b set to true if the input stream is not closed after reaching EOF */
    void setContinueReading(bool b)
    {
        readAfterEOF=b;
    }

     /** \param b set to true if the output should not be terminated with EOL*/
    void setOmitTrailingCR(bool b)
    {
        omitTrailingCR=b;
    }

    /** \param b set to true if the output should not contain a version info comment*/
    void setOmitVersionInfo(bool b)
    {
        omitVersionInfo=b;
    }

    /** \return plain outputting flag */
    bool getPlainOutput()
    {
        return ignoreFormatting;
    }

    /** \return continue reading from input stream flag */
    bool getContinueReading()
    {
        return readAfterEOF;
    }

    /** \param lineWrappingStyle wrapping style
        \param lineLength maximum length  per wrapped text line */
    void setPreformatting ( WrapMode lineWrappingStyle,unsigned int lineLength);

    /** \param b set to true if line numbers should be added */
    void setShowLineNumbers(bool flag);

    /** \param b set to true if line numbers of wrapped lines should be omitted */
    void setWrapNoNumbers(bool flag);

    /** \param b set to true if input is ASCII art codepage 437 file  */
    void setParseCodePage437(bool flag);

    /** \param b set to true if input is an ASCII art BIN file*/
    void setParseAsciiBin(bool flag);

    /** \param b set to true if input is an Tundra art BIN file*/
    void setParseAsciiTundra(bool flag);

    /** \param b set to true if K clear sequences should be ignored*/
    void setIgnoreClearSeq(bool flag);

    /** \param b set to true if CSI sequences should be ignored*/
    void setIgnoreCSISeq(bool flag);

    /** \param b set dimensions of ASCII art virtual console */
    void setAsciiArtSize(int width, int height);

    /** tell parser to use dynamic stylesheets derived from the document's formatting
       \param  flag true
    */
    void setApplyDynStyles(bool flag);

    /** Set SVG dimensions
        \param w page width
        \param h page height
    */
    void setSVGSize ( const string& w, const string& h );

    void setLineAppendage(const string& a);

protected:

    /** \param type Output type */
    CodeGenerator(ansifilter::OutputType type);

    CodeGenerator() {}

    /** \param c Character to be masked
        \return Escape sequence of output format */
    virtual string maskCharacter(unsigned char c) = 0;

    /** \param c Character to be masked
     \return Codepage 437 escape sequence of output format */
    virtual string maskCP437Character(unsigned char c) { return maskCharacter(c); }

    /** \param uri URI
     *  \param txt Description
     \return returns link formatting sequence */
    virtual string getHyperlink(std::string_view uri, std::string_view txt) {
        ostringstream os;
        os << txt << "[" << uri << "]";
        return os.str();
    }

    /** Tag for inserting line feeds*/
    string newLineTag;

    /** SVG document dimensions */
    string width, height;

    /** file input*/
    istream *in;

    /** file output*/
    ostream *out;

    /** line buffer*/
    ostringstream lineBuf;

    bool tagIsOpen; ///< a reminder to close an open tag

    string styleCommentOpen,  ///< open comment delimiter
           styleCommentClose; ///< close comment delimiter

    /** Encoding name */
    string encoding;

    /** Encoding name */
    string docTitle;

    string spacer;

    /** Test if header and footer should be omitted */
    bool fragmentOutput;

    /** The base font to use */
    string font ;

    /** The base font size to use */
    string fontSize ;

    /** Style sheet path */
    string styleSheetPath;

    /** Width of line numbers */
    unsigned int lineNumberWidth;

    /** Current line number */
    unsigned int lineNumber;
    bool showLineNumbers,     ///< show line numbers
         numberWrappedLines,  ///< also show number of wrapped lines
         numberCurrentLine,   ///< output number of current line
         addAnchors,          ///< add HTML anchor to line number
         addFunnyAnchors,     ///< add HTML links to themselves
         applyDynStyles;      ///< insert dynamic style references instead of inline styles

    bool omitVersionInfo;     ///< do not print version info comment
    bool parseCP437;          ///< treat input as CP437 file
    bool parseAsciiBin;       ///< treat input as BIN or XBIN file
    bool parseAsciiTundra;    ///< treat input as Tundra file

    /** Processes input data */
    void processInput();

    virtual void insertLineNumber ();

    /** \return true id encoding is defined */
    bool encodingDefined()
    {
        return StringTools::lowerCase(encoding)!="none";
    }

    /** convert a rgb triple to HTML color notation
    @param rgb RGB input values
    @return HTML color string
    */
    string rgb2html(unsigned char* rgb);

    string rgb2html(int r, int g, int b);

    /// 16 basic colors
    static unsigned char workingPalette[16][3];
    static unsigned char defaultPalette[16][3];

    ElementStyle elementStyle;

    vector<StyleInfo> documentStyles;

private:

    CodeGenerator(const CodeGenerator&) {}

    CodeGenerator& operator=(CodeGenerator&)
    {
        return *this;
    }

    /** parses string to extract ANSI sequence information
        @param line text line
        @param begin starting position within line
        @param end ending position within line
        @return true if sequence was recognized */
    bool parseSGRParameters(const string& line, size_t begin, size_t end);

    /** parses Codepage 437 sequence information
        @param line text line
        @param begin starting position within line
        @param end ending position within line
        */
    void parseCodePage437Seq(string line, size_t begin, size_t end);

    /** Prints document footer
        @return footer */
    virtual string getFooter() = 0;

    /** Prints document body*/
    virtual void printBody() = 0;

    /** Prints document header
        @return header
    */
    virtual string getHeader() = 0;

    /** Resets parser to origin state, call this after every file conversion */
    void reset();

    /** Output format type*/
    OutputType outputType;

    virtual string getOpenTag() = 0;  ///< returns opening formatting sequence
    virtual string getCloseTag() = 0; ///< returns closing formatting sequence

    bool ignoreFormatting; ///< ignore color and font face information
    bool readAfterEOF;     ///< continue reading after EOF occurred
    bool omitTrailingCR;   ///< do not print EOL at the end of output
    bool ignClearSeq;      ///< ignore clear sequence ESC K
    bool ignCSISeq;       ///< ignore CSIs (may interfere with UTF-8 input)

    TDChar* termBuffer;
    unsigned int curX, curY, memX, memY, maxY; ///< cursor position for Codepage 437 sequences
    unsigned int asciiArtWidth;        ///< virtual console column count
    unsigned int asciiArtHeight;       ///< virtual console line count
    unsigned int lineWrapLen; ///< max line length before wrapping

    string lineAppendage; ///< user defined end of line append string

    ElementStyle memStyle;

    /** clear line buffer
    */
    void printNewLine(bool eof=false);

    /** convert an xterm color value (0-253) to 3 unsigned chars rgb
        @param color xterm color
        @param rgb RGB output values */
    void xterm2rgb(unsigned char color, unsigned char* rgb);

    /**Print content of virtual terminal buffer. Deletes buffer. */
    void printTermBuffer();

    /**Parses BIN ASCII art file */
    void parseBinFile();

    /**Parses XBIN ASCII art file */
    void parseXBinFile();

    /**Parses Tundra ASCII art file */
    void parseTundraFile();

    /**allocate virtual terminal buffer */
    void allocateTermBuffer();

    /** @return true if stream begins with XBIN id  */
    bool streamIsXBIN();

     /** @return true if stream begins with Tundra id  */
    bool streamIsTundra();

    /// the 6 value iterations in the xterm color cube
    static const unsigned char valuerange[] ;
};

}

#endif
