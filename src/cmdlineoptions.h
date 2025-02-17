/***************************************************************************
                          cmdlineoptions.h  -  description
                             -------------------
    begin                : Sun Oct 13 2007
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

#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#include <string>
#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "enums.h"

using std::string;
using std::vector;
using std::cin;
using std::cerr;
using std::endl;

/// handle command line options

class CmdLineOptions
{
public:

    /**Constructor
     \param argc Argument count
     \param argv Argument strings
    */
    CmdLineOptions( const int argc, const char *argv[] );
    ~CmdLineOptions();

    /** \return Single output file name*/
    string getSingleOutFilename();

    /** \return Single input file name*/
    string getSingleInFilename() const;

    /** \return Output directory*/
    string getOutDirectory() ;

    /** \return Character encoding*/
    string getEncoding() const;

    /** \return font name*/
    string getFont() const;

    /** \return font size*/
    string getFontSize() const;

    /** \return path to color map*/
    string getMapPath() const;

    /** \return True if version information should be printed*/
    bool printVersion() const;

    /** \return True if help information should be printed*/
    bool printHelp() const;

    /** \return True if output should be fragmented*/
    bool fragmentOutput() const;

    /** \return output file suffix */
    string getOutFileSuffix() const;

    /** \return Line append string */
    string getLineAppendage();

    /** \return Output file format */
    ansifilter::OutputType getOutputType() const;

    /** \return True if encoding specification should be omitted in output*/
    bool omitEncoding() const;

    /** \return True if formatting infotmation should not be outputted */
    bool plainOutput() const;

    /** \return True if input files should be read after EOF occurred */
    bool ignoreInputEOF() const;

    /** \return True if line numbers should be printed */
    bool showLineNumbers() const;

    /** \return True if anchors should be added to line numbers */
    bool addAnchors() const;

    /** \return True if line numbers should be printed */
    bool wrapNoNumbers() const;

    /** \return True if input should be treated as codepage 437 ASCII art */
    bool parseCP437() const;

    /** \return True if input should be treated as BIN ASCII art */
    bool parseAsciiBin() const;

    /** \return True if input should be treated as Tundra ASCII art */
    bool parseAsciiTundra() const;

     /** \return True if output should not be terminated with carriage return */
    bool omitTrailingCR() const;

    /** \return True if output should not contain a version info comment */
    bool omitVersionInfo() const;

    /** \return True if clear sequences (ESC K) should be ignored */
    bool ignoreClearSeq() const;

        /** \return True if CSI sequences should be ignored */
    bool ignoreCSISeq() const;

    /** \return True if output should contain dynamic style classes instead of inline styles */
    bool applyDynStyles() const;

    /** \return True if dynamic styles should be saved to a file */
    bool genDynStyles() const;

    bool addFunnyAnchors() const;

    bool omitDefaultForegroundColor() const;

    /** \return Document title */
    string getDocumentTitle() const ;

    /** \return Document title */
    string getStyleSheetPath() const ;

    /** \return List of input file names*/
    const vector <string> & getInputFileNames() const;

    int getWrapLineLength() const;

    /** \return XBIN width */
    int getAsciiArtWidth() const;

    /** \return XBIN height */
    int getAsciiArtHeight() const;

    /** \return SVG width */
    string getWidth() const;

    /** \return SVG height */
    string getHeight() const;

    /** \return Allowed input file size */
    off_t getMaxFileSize() const;

private:
    ansifilter::OutputType outputType;

    bool opt_help;
    bool opt_version ;
    bool opt_fragment;
    bool opt_plain;
    bool opt_ignoreEOF;
    bool opt_linenum;
    bool opt_wrapNoNum;
    bool opt_anchors;
    bool opt_cp437;
    bool opt_asciiBin;
    bool opt_asciiTundra;

    bool opt_omit_trailing_cr;
    bool opt_omit_version_info;
    bool opt_ignoreClear;
    bool opt_ignoreCSI;

    bool opt_applyDynStyles;
    bool opt_genDynStyles;
    bool opt_funny_anchors;
    bool opt_omit_default_fg_color;

    // name of single output file
    string outFilename;
    string docTitle;
    string encodingName;
    string outDirectory;
    string font;
    string fontSize;
    string styleSheetPath;
    string colorMapPath;
    string width;
    string height;
    string lineAppendage;

    int wrapLineLen;
    int asciiArtWidth;
    int asciiArtHeight;

    off_t maxFileSize;

    /** list of all input file names */
    vector <string> inputFileNames;

    /** \return Valid path name */
    string validateDirPath(const string & path);

    /** \return directory name of path */
    string getDirName( const string & path);

    void parseRuntimeOptions( const int argc, const char *argv[], bool readInputFilenames=true);
};

#endif
