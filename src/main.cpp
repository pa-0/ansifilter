/***************************************************************************
                          main.cpp  -  description
                             -------------------

    copyright            : (C) 2007-2024 by Andre Simon
    email                : a.simon@mailbox.org

   Highlight is a universal source code to HTML converter. Syntax highlighting
   is formatted by Cascading Style Sheets. It's possible to easily enhance
   highlight's parsing database.

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

#include <memory>
#include "main.h"
#include "codegenerator.h"
#include "platform_fs.h"

using std::cout;
using std::cerr;

using std::string;
using std::unique_ptr;

void ANSIFilterApp::printVersionInfo()
{
    cout << "\n ansifilter version "
         << Info::getVersion()
         << "\n Copyright (C) 2007-2024 Andre Simon <a dot simon at mailbox.org>"
         << "\n\n Argparser class"
         << "\n Copyright (C) 2006-2008 Antonio Diaz Diaz <ant_diaz at teleline.es>"
         << "\n\n This software is released under the terms of the GNU General "
         << "Public License."
         << "\n For more information about these matters, see the file named "
         << "COPYING.\n";
}

void ANSIFilterApp::printHelp()
{
    cout << "Invocation: ansifilter [OPTION]... [FILE]...\n\n";
    cout << "ansifilter handles text files containing ANSI terminal escape codes.\n";
    cout << "\nFile handling:\n";
    cout << "  -i, --input=<file>     Name of input file (default stdin)\n";
    cout << "  -o, --output=<file>    Name of output file (default stdout)\n";
    cout << "  -O, --outdir=<dir>     Name of output directory\n";
    cout << "  -t, --tail             Continue reading after end-of-file (like tail -f)\n";
    cout << "  -x, --max-size=<size>  Set maximum input file size\n";
    cout << "                         (examples: 512M, 1G; default: 256M)\n";
    cout << "\nOutput text formats:\n";
    cout << "  -T, --text (default)   Output text\n";
    cout << "  -H, --html             Output HTML\n";
    cout << "  -M, --pango            Output Pango Markup\n";
    cout << "  -L, --latex            Output LaTeX\n";
    cout << "  -P, --tex              Output Plain TeX\n";
    cout << "  -R, --rtf              Output RTF\n";
    cout << "  -S, --svg              Output SVG\n";
    cout << "  -B, --bbcode           Output BBCode\n";
    cout << "\nFormat options:\n";
    cout << "  -a, --anchors(=self)   Add HTML line anchors (opt: self referencing, assumes -l)\n";
    cout << "  -d, --doc-title        Set HTML/LaTeX/SVG document title\n";
    cout << "  -e, --encoding=<enc>   Set HTML/RTF encoding (must match input file encoding);\n";
    cout << "                         omit encoding information if enc=NONE\n";
    cout << "  -f, --fragment         Omit HTML header and footer\n";
    cout << "  -F, --font=<font>      Set HTML/RTF/SVG font face\n";
    cout << "  -k, --ignore-clear(=0) Do not adhere to clear (ESC K) commands (default: true)\n";
    cout << "  -c, --ignore-csi       Do not adhere to CSI commands (useful for UTF-8 input)\n";
    cout << "  -A, --line-append=<s>  Output string after each output line\n";
    cout << "  -l, --line-numbers     Print line numbers in output file\n";
    cout << "  -m, --map=<path>       Read color mapping file (see README)\n";
    cout << "  -r, --style-ref=<rf>   Set HTML/TeX/LaTeX/SVG stylesheet path\n";
    cout << "  -s, --font-size=<fs>   Set HTML/RTF/SVG font size\n";
    cout << "  -p, --plain            Ignore ANSI formatting information\n";
    cout << "  -w, --wrap=<len>       Wrap long lines\n";
    cout << "  -g, --no-default-fg    Omit default foreground color\n";
    cout << "      --no-trailing-nl   Omit trailing newline\n";
    cout << "      --no-version-info  Omit version info comment\n";
    cout << "      --wrap-no-numbers  Omit line numbers of wrapped lines (assumes -l)\n";
    cout << "      --derived-styles   Output dynamic stylesheets (HTML/SVG)\n";

    cout << "\nANSI art options:\n";
    cout << "      --art-cp437        Parse codepage 437 ANSI art (HTML and RTF output)\n";
    cout << "      --art-bin          Parse BIN/XBIN ANSI art (HTML output, no stdin)\n";
    cout << "      --art-tundra       Parse Tundra ANSI art (HTML output, no stdin)\n";
    cout << "      --art-width        Set ANSI art width (default 80)\n";
    cout << "      --art-height       Set ANSI art height (default 150)\n";

    cout << "\nSVG output options:\n";
    cout << "      --height           set image height (units allowed)\n";
    cout << "      --width            set image width (see --height)\n";

    cout << "\nOther options:\n";
    cout << "  -h, --help             Print help\n";
    cout << "  -v, --version          Print version and license info\n";
    cout << "\nExamples:\n";
    cout << "ansifilter -i input.ansi -o output.txt\n";
    cout << "ansifilter *.txt\n";
    cout << "tail -f server.log | ansifilter\n\n";
    cout << "Parsing XBIN files overrides --art-width, --art-height and --map options.\n";
    cout << "The ANSI art file formats BIN, XBIN and TND cannot be read from stdin.\n";
    cout << "\nPlease report bugs to " << Info::getEmail()<< "\n";
    cout << "For updates see " << Info::getWebsite()<< "\n";
}

int ANSIFilterApp::run( const int argc, const char *argv[] )
{

    CmdLineOptions options(argc, argv);

    if (options.printVersion()) {
        printVersionInfo();
        return EXIT_SUCCESS;
    }

    if  (options.printHelp())  {
        printHelp();
        return EXIT_SUCCESS;
    }

    const  vector <string> inFileList=options.getInputFileNames();
    unique_ptr<ansifilter::CodeGenerator> generator(ansifilter::CodeGenerator::getInstance(options.getOutputType()));

    string outDirectory = options.getOutDirectory();

    unsigned int fileCount=inFileList.size(), i=0;
    string::size_type pos=0;
    string inFileName, outFilePath;
    string mapPath = options.getMapPath();
    bool failure=false;

    if (!options.omitDefaultForegroundColor()) {
        generator->setDefaultForegroundColor();
    }

    if (!generator->setColorMap(mapPath)){
        std::cerr <<"could not read map file: " << mapPath << "\n";
        return EXIT_FAILURE;
    }

    while (i < fileCount && !failure) {

        if (fileCount>1) {
            pos=(inFileList[i]).find_last_of(Platform::pathSeparator);
            inFileName = inFileList[i].substr(pos+1);
            outFilePath = outDirectory;
            outFilePath += inFileName;
            outFilePath += options.getOutFileSuffix();
        } else {
            outFilePath = options.getSingleOutFilename();
        }

        if ( inFileList[i].size() && Platform::fileSize(inFileList[i]) > options.getMaxFileSize() ) {

            std::cerr <<"file exceeds max size (see --max-size): " << inFileList[i] << "\n";
            return EXIT_FAILURE;
        }

        generator->setTitle(options.getDocumentTitle().empty()?
                            inFileList[i]:options.getDocumentTitle());

        generator->setEncoding(options.getEncoding());
        generator->setFragmentCode(options.fragmentOutput());
        generator->setPlainOutput(options.plainOutput());
        generator->setContinueReading(options.ignoreInputEOF());
        generator->setFont(options.getFont());
        generator->setFontSize(options.getFontSize());
        generator->setStyleSheet(options.getStyleSheetPath());
        generator->setPreformatting(ansifilter::WRAP_SIMPLE, options.getWrapLineLength());
        generator->setShowLineNumbers(options.showLineNumbers());
        generator->setWrapNoNumbers(!options.wrapNoNumbers());
        generator->setAddAnchors(options.addAnchors(), options.addFunnyAnchors());
        generator->setParseCodePage437(options.parseCP437());
        generator->setParseAsciiBin(options.parseAsciiBin());
        generator->setParseAsciiTundra(options.parseAsciiTundra());
        generator->setIgnoreClearSeq(options.ignoreClearSeq());
        generator->setIgnoreCSISeq(options.ignoreCSISeq());

        generator->setApplyDynStyles(options.applyDynStyles());

        generator->setAsciiArtSize(options.getAsciiArtWidth(), options.getAsciiArtHeight());
        generator->setOmitTrailingCR(options.omitTrailingCR());
        generator->setOmitVersionInfo(options.omitVersionInfo());

        generator->setSVGSize ( options.getWidth(), options.getHeight() );

        generator->setLineAppendage ( options.getLineAppendage() );

        ansifilter::ParseError error = generator->generateFile(inFileList[i], outFilePath);

        if (error==ansifilter::BAD_INPUT) {
            std::cerr << "could not read input: " << inFileList[i] << "\n";
            failure=true;
        } else if (error==ansifilter::BAD_OUTPUT) {
            std::cerr << "could not write output: " << outFilePath << "\n";
            failure=true;
        }
        ++i;
    }

    if (options.applyDynStyles() && !failure) {
        string styleStyleSheetPath = outDirectory + "derived_styles.css";
        generator->printDynamicStyleFile(styleStyleSheetPath);
    }

    return (failure) ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main( const int argc, const char *argv[] )
{
    ANSIFilterApp app;
    return app.run(argc, argv);
}
