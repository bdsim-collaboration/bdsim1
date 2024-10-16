/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *  Lexical analyzer for gmad bison parser  
 *  Ilya Agapov, 2005
 *  
*/



%x incl

/*
%option yylineno // use own improved line number info
*/

%{
  
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "parser.h"
#include "parser.tab.hh"
#include "sym_table.h"
#include "getEnv.h"
#include <unistd.h>

#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif

using namespace GMAD;

extern FILE* yyin;
extern int yyerror(const char *);

namespace GMAD {

const int max_include_depth = 10;
YY_BUFFER_STATE include_stack[max_include_depth];
std::vector<std::string> include_filename_stack(max_include_depth);
int include_linenum_stack[max_include_depth];
int include_stack_ptr = 0;

int line_num = 1;
std::string yyfilename;

}

%}

%s ERROR

%option nounput

%%

[\t ]+     //ignore whitespaces

(([0-9]+)|([0-9]*\.[0-9]*))((e|E)[+|-]?[0-9]+)? { yylval.dval=atof(yytext); return NUMBER; }


"<=" { return LE; }
">=" { return GE; }
"<>" { return NE; }
"==" { return EQ; }

"!".*$    {}    // comments - ignore
"!".*     {}    // comments at end of file (without return) - ignore

marker { return MARKER; } // reserved elements
drift { return DRIFT; }
rf { return RF; }
rfx { return RFX; }
rfy { return RFY; }
rfcavity { return RF; }
sbend { return SBEND; } 
rbend {return RBEND; }
hkicker { return HKICKER; }
vkicker { return VKICKER; }
kicker  { return KICKER;  }
tkicker { return TKICKER; }
quadrupole { return QUADRUPOLE; }
sextupole { return SEXTUPOLE; }
octupole { return OCTUPOLE; }
decapole { return DECAPOLE; }
multipole { return MULTIPOLE; }
thinmultipole { return THINMULT; }
solenoid { return SOLENOID; }
rcol { return RCOL;}
ecol { return ECOL; } 
jcol { return JCOL; }
bmcol { return BMCOL; }
gascap { return GASCAP; }
muonspoiler { return MUONSPOILER; }
muspoiler { return MUONSPOILER; }
shield {return SHIELD; }
element { return ELEMENT; }
screen { return SCREEN; }
awakescreen { return AWAKESCREEN; }
awakespectrometer { return AWAKESPECTROMETER; }
transform3d { return TRANSFORM3D ; }
laser { return LASER; }
degrader { return DEGRADER;}
gap { return GAP;}
thinrmatrix { return THINRMATRIX;}
paralleltransporter {return PARALLELTRANSPORTER;}
rmatrix {return RMATRIX;}
crystalcol { return CRYSTALCOL; }
wirescanner { return WIRESCANNER; }
undulator { return UNDULATOR; }
usercomponent { return USERCOMPONENT; }
dump { return DUMP; }
ct { return CT; }
target { return TARGET; }

cavitymodel {return CAVITYMODEL; }
cutsregion { return REGION; }
newcolour { return NEWCOLOUR; }
crystal { return CRYSTAL; }
field { return FIELD; }
placement { return PLACEMENT; }
query { return QUERY; }
samplerplacement { return SAMPLERPLACEMENT; }
scorer { return SCORER; }
scorermesh { return SCORERMESH; }
tunnel { return TUNNEL; }
xsecbias {return XSECBIAS; }
xsecBias {return XSECBIAS; }
aperture {return APERTURE; }
blm {return BLM;}
modulator {return MODULATOR;}

matdef { return MATERIAL; }
atom { return ATOM; }

line { return LINE; } 

all { return ALL; }
period { return PERIOD; }
range { return RANGE; }

"if" { return IF; }
"for" { return FOR; }
"else" { return ELSE; } 
"begin" { return BEGN; }
"end" { return END; }


include   BEGIN(incl);  //reserved commands

beam { return BEAM; }
option { return OPTION; }
beta0 { return BEAM; }  // beta0 is a synonym of beam
print { return PRINT; }
"return" { return STOP; }
stop {return STOP;}
use { return USE; }
sample { return SAMPLE; }
csample { return CSAMPLE; }

\"[^",\n]*\" {
  //strip quotes and return string
  // no commas or newline in string allowed

  // copy string without quotes, 
  // memory needs to be allocated
  yylval.str = new std::string(yytext+1,strlen(yytext)-2);
  // add string to variable vector for deletion afterwards
  Parser::Instance()->AddVariable(yylval.str);
  return STR;
}

\" {
   // give warning for other strings
   std::string errorstring = "malformed string or unmatched quote";
   yyerror(errorstring.c_str());
}

":="    { return '=';}  // alternative assignment 

[a-zA-Z#][A-Za-z0-9_#.]* {
     std::string var(yytext);
     // look up variable if defined and array or function
     // else create string (also if in the lookup map(!))
     Symtab *sp = Parser::Instance()->symlook(var);
     if (sp) {
       yylval.symp=sp;
       switch(sp->GetType()) {
       case Symtab::symtabtype::FUNCTION:
         return FUNC;
       case Symtab::symtabtype::ARRAY:
         return VECVAR;
       case Symtab::symtabtype::NUMBER:
         return NUMVAR;
       case Symtab::symtabtype::STRING:
         return STRVAR;
       default:
         break;
       }
     }
     std::string* name = new std::string(var);
     // add string to variable vector for deletion afterwards
     Parser::Instance()->AddVariable(name); 
     yylval.str = name;
     return VARIABLE;
}
.    { return yytext[0]; } // return characters like * and /

<incl>[ \t]*  // eat the whitespace
<incl>[^ \t\n;]+ {

  //this is perhaps unnecessary now...
  std::string bdsimpath = (std::string)getEnv("BDSIMPATH");
  std::string includefilename = "";
  std::string mainfilename    = yyfilename; //get the supplied main filename
  std::string mainfilepath    = "";
  if(bdsimpath.length()>0){
#ifdef BDSDEBUG
  std::cout << "parser> using BDSIMPATH to build included filepaths" << std::endl;
#endif
  includefilename = bdsimpath + (std::string)yytext;
  } else {
    // get the path part of the supplied path to the main input file
    std::string::size_type found = mainfilename.rfind("/"); // find the last '/'
    if (found != std::string::npos) {
      mainfilepath = mainfilename.substr(0,found+1); // the path is the bit before that, including the '/'
    } // else remains empty string
    // need to know whether it's an absolute or relative path
    if ((mainfilename.substr(0,1)) == "/"){
      // the main file has an absolute path
      includefilename = mainfilepath + (std::string)yytext;
    } else {
      // the main file has a relative path or just the file name
      char cwdchars[200]; //filepath up to 200 characters
      // get current working directory
      std::string cwd = (std::string)getcwd(cwdchars, sizeof(cwdchars)) + "/";
      includefilename = cwd + mainfilepath + (std::string)yytext;
    }
  }

  std::cout << "parser> reading file " << includefilename << std::endl; //yytext
  if (includefilename == yyfilename)
    {
       std::string errorstring = "\nError: recursively including the same file inside itself!\n\n Problem"; // at line.. etc
       yyerror(errorstring.c_str());
    }
  if(include_stack_ptr >= max_include_depth - 1)
    {
      std::string errorstring = "Error : Include depth exceeds " + std::to_string(max_include_depth);
      yyerror(errorstring.c_str());
    }
  else
    {
      yyin = fopen(includefilename.c_str(), "r"); //yytext
      if(yyin)
        {
          //std::cout << "saving to stack buffer n " << include_stack_ptr << ", file " << yyfilename << std::endl;
          // save info to the stack and load new buffer
          include_linenum_stack[include_stack_ptr] = line_num;
          line_num = 1;
          include_filename_stack[include_stack_ptr] = yyfilename;
          yyfilename = includefilename; //yytext
          include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
          include_filename_stack[include_stack_ptr] = yyfilename;
          yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
          //std::cout << "done saving to stack" << std::endl;
    	}
      else
        {
          std::string errorstring = "Error : can't open " + includefilename;
          yyerror(errorstring.c_str());
        }
    }
  BEGIN(INITIAL);
}

<<EOF>> {
  if (--include_stack_ptr < 0)
    {
      yyterminate();
    }
  else
    {
      // restore the previous buffer info
      //std::cout << "switching to previous buffer with " << include_filename_stack[include_stack_ptr] << std::endl;
      yy_delete_buffer(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(include_stack[include_stack_ptr]);
      yyfilename = include_filename_stack[include_stack_ptr];
      line_num = include_linenum_stack[include_stack_ptr];
    }
}

\n   line_num++; // unix line separator
\r\n line_num++; // windows line separator

%%
