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
   bison grammar for the gmad parser
*/

%{

#include "array.h"
#include "parser.h"
#include "sym_table.h"
#include "elementtype.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <list>
#include <string>
  
  using namespace GMAD;

  extern char* yytext;

  namespace GMAD {
    extern int line_num;
    extern std::string yyfilename;
  
    const int ECHO_GRAMMAR = 0; ///< print grammar rule expansion (for debugging)
    const int INTERACTIVE = 0; ///< print output of commands (like in interactive mode)
    /* for more debug with parser:
       1) set yydebug to 1 in parser.tab.cc (needs to be reset as this file gets overwritten from time to time!) 
       2) add %debug below
       3) compile bison with "-t" flag. This is automatically done when CMAKE_BUILD_TYPE equals Debug
    */

    /// bool to delay exit until full line is parsed to give possibly more meaningful message
    bool willExit = false;
    /// bool used for parsing if / else clauses correctly
    bool execute = true;
    int element_count = -1; // for samplers , ranges etc. -1 means add to all
    ElementType element_type = ElementType::_NONE; // for samplers, ranges etc.
    std::list<int>* samplerPartIDList = nullptr;

    /// helper method for undeclared variables
    void undeclaredVariable(std::string s)
    {
      std::string errorstring = "ERROR: " + s + " is not declared";
      yyerror(errorstring.c_str());
    }
  }
%}

/* define stack type */

%union{
  double dval;
  int ival; // ElementType, but underlying type as it is not possible to have enum class in union, rely on static_casts
  GMAD::Symtab *symp;
  std::string *str;
  GMAD::Array *array;
}

/* more debug output can be added with %debug" */
//%debug

// change associativity of operators
%left '+' '-'
%left '*' '/'
%left '^' '<' '>' NE LE GE EQ
%nonassoc UMINUS
%nonassoc UPLUS

// declare union value of tokens and types
%token <dval> NUMBER
%token <symp> NUMVAR STRVAR VECVAR FUNC
%token <str> STR VARIABLE
%token <ival> MARKER ELEMENT DRIFT RF RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE
%token <ival> DECAPOLE MULTIPOLE SCREEN AWAKESCREEN AWAKESPECTROMETER THINMULT
%token <ival> SOLENOID RCOL JCOL ECOL BMCOL GASCAP GASJET LINE LASER TRANSFORM3D MUONSPOILER MUSPOILER
%token <ival> SHIELD DEGRADER GAP CRYSTALCOL WIRESCANNER
%token <ival> VKICKER HKICKER KICKER TKICKER THINRMATRIX PARALLELTRANSPORTER
%token <ival> RMATRIX UNDULATOR USERCOMPONENT DUMP CT TARGET RFX RFY
%token ALL ATOM MATERIAL PERIOD XSECBIAS REGION PLACEMENT NEWCOLOUR SAMPLERPLACEMENT
%token SCORER SCORERMESH BLM MODULATOR
%token CRYSTAL FIELD CAVITYMODEL QUERY TUNNEL APERTURE
%token BEAM OPTION PRINT RANGE STOP USE SAMPLE CSAMPLE
%token IF ELSE BEGN END LE GE NE EQ FOR

%type <dval> aexpr expr
%type <symp> assignment symdecl symbol symdef
%type <array> vecexpr
%type <array> vectnum vectstr
%type <str> use_parameters
%type <ival> component component_with_params newinstance
%type <str> sample_options
%type <str> paramassign string

/* printout format for debug output */
/*
%printer { fprintf (yyoutput, "%.10g", $$); } <dval>
%printer { fprintf (yyoutput, "%d", $$); } <ival>
%printer { fprintf (yyoutput, "\"%s\"", $$); } <str>
%printer { fprintf (yyoutput, "\"%s\"", $$->name); } <symp>
%printer { fprintf (yyoutput, "size %d, &%p", $$->size, (void*)$$->data); } <array>
%printer { fprintf (yyoutput, "<>"); } <>
*/

%%

// every statement ends in a semicolon 
input : 
      | input stmt ';'
       {
         if(ECHO_GRAMMAR) std::cout << "input -> input stmt ';' " << std::endl;
         if(willExit) yyerror("Error");
       }

// deconstruct statements into atomic statements
stmt :        if_clause '{' stmt '}'
              {
                if(ECHO_GRAMMAR) std::cout << "stmt -> IF '(' aexpr ')' stmt" << std::endl;
                execute = true; // reset execute after clause
              }
              | if_clause '{' stmt '}' else_clause '{' stmt '}' 
              {
                if(ECHO_GRAMMAR) std::cout << "stmt -> IF '(' bool_expr ')' ELSE stmt" << std::endl;
                execute = true; // reset execute after clause
              }
              | atomic_stmt    { if(ECHO_GRAMMAR) std::cout << "stmt -> atomic_stmt" << std::endl; }
              | BEGN input END { if(ECHO_GRAMMAR) std::cout << "stmt -> '{' stmt ';' atomic_stmt '}'" << std::endl;}

if_clause: IF '(' aexpr ')' {if( ($3 > 0) && execute ) execute = true; else execute = false;}

else_clause: ELSE
             {
               if (execute) {execute = false;}
               else {execute = true;}
             }

// atomic statements can be empty, a mathematical expression, a declaration or a command
atomic_stmt : 
            | expr    { if(ECHO_GRAMMAR) std::cout << "atomic_stmt -> expr" << std::endl; }
            | command { if(ECHO_GRAMMAR) std::cout << "atomic_stmt -> command" << std::endl; }
            | decl    { if(ECHO_GRAMMAR) std::cout << "atomic_stmt -> decl" << std::endl; }

// instantiate an object
decl : VARIABLE ':' component_with_params
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *$1 << " : " << static_cast<ElementType>($3) << std::endl;
             // check parameters and write into element table
             Parser::Instance()->write_table($1,static_cast<ElementType>($3));
             Parser::Instance()->ClearParams();
         }
     }
     | VARIABLE ':' MARKER
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *$1 << " : " << ElementType::_MARKER << std::endl;
             // check parameters and write into element table
             Parser::Instance()->write_table($1,ElementType::_MARKER);
             Parser::Instance()->ClearParams();
         }
     }
     | VARIABLE ':' line 
     {
         if(execute) {
             // copy tmp_list to params
             Parser::Instance()->write_table($1,ElementType::_LINE,true);
         }
     }
     | VARIABLE ':' newinstance
     {
         if(execute) {
             ElementType type = static_cast<ElementType>($3);
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << *($1) << " : " << type << std::endl;
             if(type != ElementType::_NONE) {
                 Parser::Instance()->write_table($1,type);
             }
             Parser::Instance()->ClearParams();
         }
     }
     // extend an existing object
     | VARIABLE ':' extend_options
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "edit : EXTENDING   -- " << *($1) << std::endl;
             Parser::Instance()->Overwrite(*$1);
         }
     }
     | VARIABLE ':' atom
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : atom" << std::endl;
             Parser::Instance()->SetValue<Atom>("name",*($1));
             Parser::Instance()->Add<Atom>(true, "atom");
         }
     }
     | VARIABLE ':' material
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : material" << std::endl;
             Parser::Instance()->SetValue<Material>("name",*($1));
             Parser::Instance()->Add<Material>(true, "material");
         }
     }
     | VARIABLE ':' tunnel
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : tunnel" << std::endl;
             Parser::Instance()->SetValue<Tunnel>("name",*($1));
             Parser::Instance()->Add<Tunnel>(true, "tunnel");
         }
     }
     | VARIABLE ':' region
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : region" << std::endl;
             Parser::Instance()->SetValue<Region>("name",*($1));
             Parser::Instance()->Add<Region>(true, "region");
         }
     }
     | VARIABLE ':' placement
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : placement" << std::endl;
             Parser::Instance()->SetValue<Placement>("name",*($1));
             Parser::Instance()->Add<Placement>(true, "placement");
         }
     }
     | VARIABLE ':' scorer
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : scorer" << std::endl;
             Parser::Instance()->SetValue<Scorer>("name",*($1));
             Parser::Instance()->Add<Scorer>(true, "scorer");
         }
     }
     | VARIABLE ':' scorermesh
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : scorermesh" << std::endl;
             Parser::Instance()->SetValue<ScorerMesh>("name",*($1));
             Parser::Instance()->Add<ScorerMesh>(true, "scorermesh");
         }
     }
     | VARIABLE ':' samplerplacement
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : samplerplacement" << std::endl;
             Parser::Instance()->SetValue<SamplerPlacement>("name",*($1));
             Parser::Instance()->Add<SamplerPlacement>(true, "samplerplacement");
         }
     }
     | VARIABLE ':' blm
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : blm" << std::endl;
             Parser::Instance()->SetValue<BLMPlacement>("name",*($1));
             Parser::Instance()->Add<BLMPlacement>(true, "blm");
         }
     }
     | VARIABLE ':' modulator
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : modulator" << std::endl;
             Parser::Instance()->SetValue<Modulator>("name", *($1));
             Parser::Instance()->Add<Modulator>(true, "modulator");
         }
     }
     | VARIABLE ':' query
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : query" << std::endl;
             Parser::Instance()->SetValue<Query>("name", *($1));
             Parser::Instance()->Add<Query>(true, "query");
         }
     }
      | VARIABLE ':' newcolour
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : newcolour" << std::endl;
             Parser::Instance()->SetValue<NewColour>("name", *($1));
             Parser::Instance()->Add<NewColour>(true, "newcolour");
         }
     }
      | VARIABLE ':' crystal
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : crystal" << std::endl;
             Parser::Instance()->SetValue<Crystal>("name", *($1));
             Parser::Instance()->Add<Crystal>(true, "crystal");
         }
     }
     | VARIABLE ':' field
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : field" << std::endl;
             Parser::Instance()->SetValue<Field>("name", *($1));
             Parser::Instance()->Add<Field>(true, "field");
         }
     }
     | VARIABLE ':' cavitymodel
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : cavitymodel" << std::endl;
             Parser::Instance()->SetValue<CavityModel>("name",*($1));
             Parser::Instance()->Add<CavityModel>(true, "cavitymodel");
         }
     }
     | VARIABLE ':' xsecbias
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : xsecbias" << std::endl;
             Parser::Instance()->SetValue<PhysicsBiasing>("name",*($1));
             Parser::Instance()->Add<PhysicsBiasing,FastList<PhysicsBiasing>>(true,"xsecbias");
         }
     }
     | VARIABLE ':' aperture
     {
         if(execute) {
             if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : aperture" << std::endl;
             Parser::Instance()->SetValue<Aperture>("name",*($1));
             Parser::Instance()->Add<Aperture>(true, "aperture");
         }
     }
     | VARIABLE ':' component
     {
         if(execute) {
             yyerror("ERROR: Element needs parameters");
         }
     }
     | VARIABLE ':' object_noparams
     {
         if(execute) {
             yyerror("ERROR: Object needs parameters");
         }
     }
     | symdef
     {
         if(execute) {
             std::string errorstring = "ERROR: " + $1->GetName() + " is already defined or a reserved name or function\n";
             yyerror(errorstring.c_str());
         }
     }

symdef : FUNC ':'   { $$ = $1; }
       | NUMVAR ':' { $$ = $1; }
       | STRVAR ':' { $$ = $1; }

component_with_params : component ',' parameters

component : DRIFT       {$$=static_cast<int>(ElementType::_DRIFT);}
          | RF          {$$=static_cast<int>(ElementType::_RF);}
          | SBEND       {$$=static_cast<int>(ElementType::_SBEND);}
          | RBEND       {$$=static_cast<int>(ElementType::_RBEND);}
          | VKICKER     {$$=static_cast<int>(ElementType::_VKICKER);}
          | HKICKER     {$$=static_cast<int>(ElementType::_HKICKER);}
          | KICKER      {$$=static_cast<int>(ElementType::_KICKER);}
          | TKICKER     {$$=static_cast<int>(ElementType::_TKICKER);}
          | QUADRUPOLE  {$$=static_cast<int>(ElementType::_QUAD);}
          | SEXTUPOLE   {$$=static_cast<int>(ElementType::_SEXTUPOLE);}
          | OCTUPOLE    {$$=static_cast<int>(ElementType::_OCTUPOLE);}
          | DECAPOLE    {$$=static_cast<int>(ElementType::_DECAPOLE);}
          | MULTIPOLE   {$$=static_cast<int>(ElementType::_MULT);}
          | THINMULT    {$$=static_cast<int>(ElementType::_THINMULT);}
          | SOLENOID    {$$=static_cast<int>(ElementType::_SOLENOID);}
          | ECOL        {$$=static_cast<int>(ElementType::_ECOL);}
          | JCOL        {$$=static_cast<int>(ElementType::_JCOL);}
          | RCOL        {$$=static_cast<int>(ElementType::_RCOL);}
          | BMCOL       {$$=static_cast<int>(ElementType::_BMCOL);}
          | GASCAP      {$$=static_cast<int>(ElementType::_GASCAP);}
          | GASJET      {$$=static_cast<int>(ElementType::_GASJET);}
          | MUONSPOILER {$$=static_cast<int>(ElementType::_MUONSPOILER);}
          | MUSPOILER   {$$=static_cast<int>(ElementType::_MUONSPOILER);}
          | SHIELD      {$$=static_cast<int>(ElementType::_SHIELD);}
          | DEGRADER    {$$=static_cast<int>(ElementType::_DEGRADER);}
          | WIRESCANNER {$$=static_cast<int>(ElementType::_WIRESCANNER);}
          | GAP         {$$=static_cast<int>(ElementType::_GAP);}
          | CRYSTALCOL  {$$=static_cast<int>(ElementType::_CRYSTALCOL);}
          | LASER       {$$=static_cast<int>(ElementType::_LASER);}
          | SCREEN      {$$=static_cast<int>(ElementType::_SCREEN);}
          | AWAKESCREEN {$$=static_cast<int>(ElementType::_AWAKESCREEN);}
          | AWAKESPECTROMETER {$$=static_cast<int>(ElementType::_AWAKESPECTROMETER);}
          | TRANSFORM3D {$$=static_cast<int>(ElementType::_TRANSFORM3D);}
          | ELEMENT     {$$=static_cast<int>(ElementType::_ELEMENT);}
          | THINRMATRIX {$$=static_cast<int>(ElementType::_THINRMATRIX);}
          | PARALLELTRANSPORTER {$$=static_cast<int>(ElementType::_PARALLELTRANSPORTER);}
          | RMATRIX     {$$=static_cast<int>(ElementType::_RMATRIX);}
          | UNDULATOR   {$$=static_cast<int>(ElementType::_UNDULATOR);}
          | USERCOMPONENT {$$=static_cast<int>(ElementType::_USERCOMPONENT);}
          | DUMP        {$$=static_cast<int>(ElementType::_DUMP);}
          | CT          {$$=static_cast<int>(ElementType::_CT);}
          | TARGET      {$$=static_cast<int>(ElementType::_TARGET);}
          | RFX         {$$=static_cast<int>(ElementType::_RFX);}
          | RFY         {$$=static_cast<int>(ElementType::_RFY);}

atom        : ATOM        ',' atom_options
material    : MATERIAL    ',' material_options
region      : REGION      ',' region_options
placement   : PLACEMENT   ',' placement_options
newcolour   : NEWCOLOUR   ',' colour_options
crystal     : CRYSTAL     ',' crystal_options
field       : FIELD       ',' field_options
cavitymodel : CAVITYMODEL ',' cavitymodel_options
query       : QUERY       ',' query_options
tunnel      : TUNNEL      ',' tunnel_options
xsecbias    : XSECBIAS    ',' xsecbias_options
samplerplacement : SAMPLERPLACEMENT ',' samplerplacement_options
scorer      : SCORER      ',' scorer_options
scorermesh  : SCORERMESH  ',' scorermesh_options
aperture    : APERTURE    ',' aperture_options
blm         : BLM         ',' blm_options
modulator   : MODULATOR   ',' modulator_options

// every object needs parameters
object_noparams : MATERIAL
                | ATOM
                | REGION
                | PLACEMENT
                | NEWCOLOUR
                | CRYSTAL
                | FIELD
                | CAVITYMODEL
                | QUERY
                | TUNNEL
                | XSECBIAS
                | SAMPLERPLACEMENT
                | SCORER
                | SCORERMESH
                | APERTURE
                | BLM
                | MODULATOR

newinstance : VARIABLE ',' parameters
            {
              if(execute) {
                $$ = Parser::Instance()->copy_element_to_params(*$1);
              }
            }
            | VARIABLE
            {
              if(execute) {
                $$ = Parser::Instance()->copy_element_to_params(*$1);
              }
            }

paramassign : VARIABLE
            {
              $$=$1;
            }
            | symbol
            {
              $$ = new std::string($1->GetName());
              // store to prevent leak
              Parser::Instance()->AddVariable($$);
            }
            | STRVAR
            {
              $$ = new std::string($1->GetName());
              Parser::Instance()->AddVariable($$);
            }

// reduce STR and STRVAR
string : STR
       {
         $$ = $1;
       }
       | STRVAR
       {
         $$ = new std::string($1->GetString());
         // store to prevent leak
         Parser::Instance()->AddVariable($$);
       }

parameters_extend : /* nothing */
                  | ',' parameters

parameters: paramassign '=' aexpr parameters_extend
          {
              if(execute)
                  Parser::Instance()->SetValue<Parameters>(*($1),$3);
          }
          | paramassign '=' vecexpr parameters_extend
          {
              if(execute) 
                  Parser::Instance()->SetValue<Parameters>(*($1),$3);
          }
          | paramassign '=' string parameters_extend
          {
              if(execute)
                  Parser::Instance()->SetValue<Parameters>(*($1),*$3);
          }

line : LINE '=' '(' element_seq ')'

line : LINE '=' '-' '(' rev_element_seq ')'

element_seq_extend : /* nothing */
                   | ',' element_seq

element_seq : 
            | VARIABLE element_seq_extend
              {
                if(execute) Parser::Instance()->add_element_temp(*($1), 1, true, ElementType::_LINE);
              }
            | VARIABLE '*' NUMBER element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
            }
            | NUMBER '*' VARIABLE element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
            }
            | '-' VARIABLE element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
            }

rev_element_seq_extend : /* nothing */
                       | ',' rev_element_seq

rev_element_seq : 
            | VARIABLE rev_element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
            }
            | VARIABLE '*' NUMBER rev_element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
            }
            | NUMBER '*' VARIABLE rev_element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
            }
            | '-' VARIABLE rev_element_seq_extend
            {
              if(execute) Parser::Instance()->add_element_temp((*$2), 1, false, ElementType::_LINE);
            }

expr : aexpr 
     {
       if(ECHO_GRAMMAR) std::cout << "expr -> aexpr" << std::endl;
       if(execute) 
         {
           if(INTERACTIVE) std::cout << "\t" << $1 << std::endl;
           $$=$1;
         }
     }
     | vecexpr 
     {
       if(ECHO_GRAMMAR) std::cout << "expr -> vecexpr" << std::endl;
       if(execute)
         {
           if(INTERACTIVE)
             {$1->Print();}
           $$ = 0;
         }
     }
     | assignment 
     { // check type
       if(ECHO_GRAMMAR) std::cout << "expr -> assignment" << std::endl;
       if(execute)
         {
           if(INTERACTIVE) {
             $1->Print();
           }
           $$=0;
         }
     }

aexpr  : NUMBER               { $$ = $1;                         }
       | symbol               { $$ = $1->GetNumber();            }
       | FUNC '(' aexpr ')'   { $$ = $1->GetFunction()($3);      }
       | aexpr '+' aexpr      { $$ = $1 + $3;                    }
       | aexpr '-' aexpr      { $$ = $1 - $3;                    }  
       | aexpr '*' aexpr      { $$ = $1 * $3;                    }
       | aexpr '/' aexpr      { $$ = $1 / $3;                    }
       | aexpr '^' aexpr      { $$ = std::pow($1,$3);            }
       | '-' aexpr  %prec UMINUS { $$ = -$2; }
       | '+' aexpr  %prec UPLUS { $$ = $2; }
       | '(' aexpr ')'         { $$ = $2;                        }
       | '<' vecexpr ',' vecexpr '>' // scalar product
         {
           $$ = $2->Product($4);
         } 
       // boolean stuff
       | aexpr '<' aexpr { $$ = ($1 < $3 )? 1 : 0; }
       | aexpr LE aexpr { $$ = ($1 <= $3 )? 1 : 0; }
       | aexpr '>' aexpr { $$ = ($1 > $3 )? 1 : 0; }
       | aexpr GE aexpr { $$ = ($1 >= $3 )? 1 : 0; }
       | aexpr NE aexpr { $$ = ($1 != $3 )? 1 : 0; }
       | aexpr EQ aexpr { $$ = ($1 == $3 )? 1 : 0; }
       // undeclared variable
       | VARIABLE
         {
           if(execute) {
             undeclaredVariable(*$1);
           }
         }

// numerical symbol (could change to include string symbols (aka STRVAR))
symbol  : OPTION '[' string ']' // option attributes
        {
          if(ECHO_GRAMMAR) std::cout << "aexpr-> OPTION [ " << *($3) << " ]" << std::endl;
          if (execute)
            {
              double value = Parser::Instance()->GetValue<Options>(*($3));
              // create symtable if not already defined
              std::string symname = "option_" + *($3);
              Symtab *sp = Parser::Instance()->symlook(symname);
              if (!sp) {
                sp = Parser::Instance()->symcreate(symname);
              }
              sp->Set(value);
              $$ = sp;
            }
        }
        | VARIABLE '[' string ']' // element attributes
        {
          if(ECHO_GRAMMAR) std::cout << "aexpr-> " << *($1) << " [ " << *($3) << " ]" << std::endl;
          if (execute)
            {
              double value = Parser::Instance()->property_lookup(*($1),*($3));
              // create symtable if not already defined
              std::string symname = "element_" + *($1) + "_" + *($3);
              Symtab *sp = Parser::Instance()->symlook(symname);
              if (!sp) {
                sp = Parser::Instance()->symcreate(symname);
              }
              sp->Set(value);
              $$ = sp;
            }
        }
        | NUMVAR {$$ = $1;}

symdecl : VARIABLE '='
        {
          if(execute)
            {
              std::string errorReason;
              if (Parser::Instance()->InvalidSymbolName(*($1), errorReason))
                {yyerror(errorReason.c_str());}
              Symtab *sp = Parser::Instance()->symcreate(*($1));
              $$ = sp;
            }
        }
        | symbol '='
        {
          if(execute)
            {
              std::cout << "WARNING redefinition of variable " << $1->GetName() << " (old value: " << $1->GetNumber()
                        << ") on line " << GMAD::line_num << std::endl;
              $$ = $1;
            }
        }
        | STRVAR '='
        {
          if(execute)
            {
              std::cout << "WARNING redefinition of variable " << $1->GetName() << " (old value: " << $1->GetString()
                        << ") on line " << GMAD::line_num << std::endl;
              $$ = $1;
            }
        }
        | VECVAR '='
        {
          if(execute)
            {
              std::cout << "WARNING redefinition of array variable " << $1->GetName() << " on line " << GMAD::line_num << std::endl;
              $$=$1;
            }
        }

assignment :  symdecl aexpr  
           {
             if(ECHO_GRAMMAR) std::cout << $1->GetName() << std::endl;
             if(execute)
               {
                 if($1->IsReserved()) {
                   std::string errorstring = "ERROR: " + $1->GetName() + " is reserved\n";
                   yyerror(errorstring.c_str());
                 }
                 else
                   {
                     $1->Set($2);
                     $$=$1;
                   }
               }
           }
           |  symdecl STR
           {
             if (execute)
               {
                 if($1->IsReserved()) {
                   std::string errorstring = "ERROR: " + $1->GetName() + " is reserved\n";
                   yyerror(errorstring.c_str());
                 }
                 else
                   {
                     $1->Set(*$2);
                     $$=$1;
                   }
               }
           }
           |  symdecl vecexpr
           {
             if(execute)
               {
                 $1->Set($2);
                 $$=$1;
               }
           }

vecexpr : VECVAR              {if(execute) $$ = new Array($1);} 
        | vectnum             {if(execute) $$ = $1;}
        | vectstr             {if(execute) $$ = $1;}
        | vecexpr '+' vecexpr {if(execute) $$ = Array::Add($1,$3);}
        | vecexpr '-' vecexpr {if(execute) $$ = Array::Subtract($1,$3);}
        | vecexpr '+' aexpr   {if(execute) $$ = Array::Add($1,$3);}
        | aexpr   '+' vecexpr {if(execute) $$ = Array::Add($3,$1);}
        | vecexpr '*' aexpr   {if(execute) $$ = Array::Multiply($1,$3);}
        | aexpr   '*' vecexpr {if(execute) $$ = Array::Multiply($3,$1);}
        | vecexpr '/' aexpr   {if(execute) $$ = Array::Divide($1,$3);}
        | vecexpr '-' aexpr   {if(execute) $$ = Array::Subtract($1,$3);}
        | aexpr '-' vecexpr
        {
          if(execute)
            {
              Array* a = Array::Multiply($3,-1);
              $$ = Array::Add(a,$1);
            }
        }

vectnumexec : '{' numbers '}'
            | '[' numbers ']'

vectnum : vectnumexec
        {
          if(execute)
            {
              $$ = new Array;
              Parser::Instance()->FillArray($$);
              Parser::Instance()->FillString($$);		
            }
        }

vectstrexec : '[' letters ']'
            | '{' letters '}'

vectstr : vectstrexec
	      {
          if(execute)
            {
              $$ = new Array;
              Parser::Instance()->FillString($$);
            }
        }

numbers : aexpr ',' numbers { if(execute) Parser::Instance()->Store($1);} 
        | aexpr             { if(execute) Parser::Instance()->Store($1);}

letters : string ',' letters { if(execute) Parser::Instance()->Store(*$1);}
        | string             { if(execute) Parser::Instance()->Store(*$1);}

// accept print with and without comma
print   : PRINT
        | PRINT ','

command : STOP         { if(execute) Parser::Instance()->quit(); }
        | print        { if(execute) Parser::Instance()->PrintElements(); }
        | print LINE   { if(execute) Parser::Instance()->PrintBeamline(); }
        | print OPTION { if(execute) Parser::Instance()->PrintOptions(); }
        | print VARIABLE
          {
            if(execute)
	      {
		Symtab* sp = Parser::Instance()->symlook(*($2));
		if (!sp)
		  {
		    bool printedObjectOk = Parser::Instance()->TryPrintingObject(*($2));
		    if (!printedObjectOk)
		      {
			// variable not defined, maybe an element? (will exit if not)
			const Element& element = Parser::Instance()->find_element(*($2));
			element.print();
		      }
		  }
		else
		  {sp->Print();}
	      }
          }
        | print STR    { if(execute) std::cout << *($2) << std::endl;}
        | print symbol { if(execute) $2->Print();}
        | print STRVAR { if(execute) $2->Print();}
        | print VECVAR { if(execute) $2->Print();}
        | USE ',' use_parameters { if(execute) Parser::Instance()->expand_line(Parser::Instance()->current_line,Parser::Instance()->current_start, Parser::Instance()->current_end);}
        | OPTION ',' option_parameters
        | BEAM   ',' beam_parameters
        | SAMPLE ',' sample_options 
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> SAMPLE" << std::endl;
              Parser::Instance()->add_sampler(*($3), element_count, element_type, "plane", samplerPartIDList);
              element_count = -1;
              Parser::Instance()->ClearParams();
              delete samplerPartIDList; samplerPartIDList = nullptr;
            }
        }
        | CSAMPLE ',' sample_options
        {
	  if(execute)
	    {
	      if(ECHO_GRAMMAR) std::cout << "command -> CSAMPLE" << std::endl;
	      Parser::Instance()->add_sampler(*($3), element_count, element_type, "cylinder", samplerPartIDList);
	      element_count = -1;
	      Parser::Instance()->ClearParams();
	      delete samplerPartIDList; samplerPartIDList = nullptr;
	    }
	}
        | ATOM ',' atom_options // atom
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> ATOM" << std::endl;
              Parser::Instance()->Add<Atom>(true, "atom");
            }
        }
        | MATERIAL ',' material_options // material
        {
          if(execute)
            {  
              if(ECHO_GRAMMAR) std::cout << "command -> MATERIAL" << std::endl;
              Parser::Instance()->Add<Material>(true, "material");
            }
        }
        | TUNNEL ',' tunnel_options // tunnel
        {
          if(execute)
            {
                if(ECHO_GRAMMAR) std::cout << "command -> TUNNEL" << std::endl;
                Parser::Instance()->Add<Tunnel>(true, "tunnel");
            }
        }
        | REGION ',' region_options // region
        {
          if(execute)
            {  
              if(ECHO_GRAMMAR) std::cout << "command -> REGION" << std::endl;
              Parser::Instance()->Add<Region>(true, "region");
            }
        }
        | PLACEMENT ',' placement_options // placement
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> PLACEMENT" << std::endl;
              Parser::Instance()->Add<Placement>(true, "placement");
            }
        }
        | SAMPLERPLACEMENT ',' samplerplacement_options // placement
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> SAMPLERPLACEMENT" << std::endl;
              Parser::Instance()->Add<SamplerPlacement>(true, "samplerplacement");
            }
        }
        | SCORER ',' scorer_options // placement
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> SCORER" << std::endl;
              Parser::Instance()->Add<Scorer>(true, "scorer");
            }
        }
        | SCORERMESH ',' scorermesh_options // placement
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> SCORERMESH" << std::endl;
              Parser::Instance()->Add<ScorerMesh>(true, "scorermesh");
            }
        }
        | BLM ',' blm_options // blm
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> BLM" << std::endl;
              Parser::Instance()->Add<BLMPlacement>(true, "blm");
            }
        }
        | MODULATOR ',' modulator_options // modulator
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> MODULATOR" << std::endl;
              Parser::Instance()->Add<Modulator>(true, "modulator");
            }
        }
        | NEWCOLOUR ',' colour_options // colour
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> NEWCOLOUR" << std::endl;
              Parser::Instance()->Add<NewColour>(true, "newcolour");
            }
        }
        | CRYSTAL ',' crystal_options // crystal
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> CRYSTAL" << std::endl;
              Parser::Instance()->Add<Crystal>(true, "crystal");
            }
        }
        | FIELD ',' field_options // field
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> FIELD" << std::endl;
              Parser::Instance()->Add<Field>(true, "field");
            }
        }
        | CAVITYMODEL ',' cavitymodel_options // cavitymodel
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> CAVITYMODEL" << std::endl;
              Parser::Instance()->Add<CavityModel>(true, "cavitymodel");
            }
        }
        | QUERY ',' query_options // query
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> QUERY" << std::endl;
              Parser::Instance()->Add<Query>(true, "query");
            }
        }
        | XSECBIAS ',' xsecbias_options // xsecbias
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> XSECBIAS" << std::endl;
              Parser::Instance()->Add<PhysicsBiasing,FastList<PhysicsBiasing>>(true, "xsecbias");
            }
        }
        | APERTURE ',' aperture_options // aperture
        {
          if(execute)
            {
              if(ECHO_GRAMMAR) std::cout << "command -> APERTURE" << std::endl;
              Parser::Instance()->Add<Aperture>(true, "aperture");
            }
        }

use_parameters :  VARIABLE
               {
                 if(execute)
                   {
                     $$ = $1;
                     Parser::Instance()->current_line = (*$1);
                     Parser::Instance()->current_start = "";
                     Parser::Instance()->current_end = "";
                   }
               }
               | PERIOD '=' VARIABLE
               {
                 if(execute)
                   {
                     $$ = $3;
                     Parser::Instance()->current_line = *($3);
                     Parser::Instance()->current_start = "";
                     Parser::Instance()->current_end = "";
                   }
               }
               | PERIOD '=' VARIABLE ',' RANGE '=' VARIABLE '/' VARIABLE
               {
                 if(execute)
                   {
                     $$ = $3;
                     Parser::Instance()->current_line = *($3);
                     Parser::Instance()->current_start = *($7);
                     Parser::Instance()->current_end = *($9);
                   }
               }

sample_options_extend : /* nothing */
                      | ',' sample_options

sample_options: RANGE '=' VARIABLE sample_options_extend
              {
                if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << *($3) << std::endl;
                if(execute) $$ = $3;
              }
              | RANGE '=' VARIABLE '[' NUMBER ']' sample_options_extend
              {
                if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << *($3) << " [" << $5 << "]" << std::endl;
                if(execute)
                  {
                    $$ = $3;
                    element_count = (int)$5;
                  }
              }
              | ALL sample_options_extend
              {
                if(ECHO_GRAMMAR) std::cout << "sample_opt, all" << std::endl;
                // -2: convention to add to all elements
                // empty name so that element name can be attached
                if(execute)
                  {
                    $$ = new std::string("");
                    element_count = -2;
                    element_type=ElementType::_NONE;
                  }
              }
	      | component sample_options_extend
              {
                if(ECHO_GRAMMAR) std::cout << "sample_opt, component " << typestr(static_cast<ElementType>($1)) << std::endl;
                if(execute)
                  {
                    element_type = static_cast<ElementType>($1);
                    element_count = -2;
                    $$ = new std::string("");
                  }
              }
              | paramassign '=' vecexpr sample_options_extend
              {//not extend in this rule as it should come last
                if(ECHO_GRAMMAR) std::cout << "sample_opt, vecexpr " << std::endl;
                if(execute)
                  {
                    std::string parameterName = std::string(*$1);
                    if (parameterName != "partID")
                      {throw std::invalid_argument("invalid sampler parameter " + parameterName);}
                    samplerPartIDList = Parser::Instance()->ArrayToList<int>($3);
                  }
              }

cavitymodel_options_extend : /* nothing */
                           | ',' cavitymodel_options

cavitymodel_options : paramassign '=' aexpr cavitymodel_options_extend
                      { if(execute) Parser::Instance()->SetValue<CavityModel>((*$1),$3);}
                    | paramassign '=' string cavitymodel_options_extend
                      { if(execute) Parser::Instance()->SetValue<CavityModel>( *$1,*$3);}

material_options_extend : /* nothing */
                        | ',' material_options

material_options : paramassign '=' aexpr material_options_extend
                    { if(execute) Parser::Instance()->SetValue<Material>((*$1),$3);}
                 | paramassign '=' string material_options_extend
                    { if(execute) Parser::Instance()->SetValue<Material>(*$1,*$3);}
                 | paramassign '=' vecexpr material_options_extend
                    { if(execute) Parser::Instance()->SetValue<Material>(*($1),$3);}

atom_options_extend : /* nothing */
                    | ',' atom_options

atom_options : paramassign '=' aexpr atom_options_extend
               { if(execute) Parser::Instance()->SetValue<Atom>((*$1),$3);}
             | paramassign '=' string atom_options_extend
               { if(execute) Parser::Instance()->SetValue<Atom>(*$1,*$3);}

region_options_extend : /* nothing */
                      | ',' region_options

region_options : paramassign '=' aexpr region_options_extend
                 { if(execute) Parser::Instance()->SetValue<Region>((*$1),$3);}
               | paramassign '=' string region_options_extend
                 { if(execute) Parser::Instance()->SetValue<Region>(*$1,*$3);}

placement_options_extend : /* nothing */
                         | ',' placement_options

placement_options : paramassign '=' aexpr placement_options_extend
                    { if(execute) Parser::Instance()->SetValue<Placement>((*$1),$3);}
                  | paramassign '=' string placement_options_extend
                    { if(execute) Parser::Instance()->SetValue<Placement>(*$1,*$3);}

samplerplacement_options_extend : /* nothing */
                         | ',' samplerplacement_options

samplerplacement_options : paramassign '=' aexpr samplerplacement_options_extend
                    { if(execute) Parser::Instance()->SetValue<SamplerPlacement>((*$1),$3);}
                  | paramassign '=' string samplerplacement_options_extend
                    { if(execute) Parser::Instance()->SetValue<SamplerPlacement>(*$1,*$3);}
                  | paramassign '=' vecexpr samplerplacement_options_extend
                    {
		      if(execute)
                        {
                          Parser::Instance()->SetValue<SamplerPlacement>(*($1),$3);
                          if (*($1) == "partID")
                          {// manually register the particle filter set
                             std::list<int>* partIDSet = Parser::Instance()->ArrayToList<int>($3);
                             int setID = Parser::Instance()->add_sampler_partIDSet(partIDSet);
                             Parser::Instance()->GetGlobal<SamplerPlacement>().partIDSetID = setID;
                             //Parser::Instance()->SetValue<SamplerPlacement>("partIDSetID", setID);
                          }
                        }
                   }
scorer_options_extend : /* nothing */
                  | ',' scorer_options

scorer_options : paramassign '=' aexpr scorer_options_extend
                    { if(execute) Parser::Instance()->SetValue<Scorer>((*$1),$3);}
                  | paramassign '=' string scorer_options_extend
                    { if(execute) Parser::Instance()->SetValue<Scorer>(*$1,*$3);}

scorermesh_options_extend : /* nothing */
                  | ',' scorermesh_options

scorermesh_options : paramassign '=' aexpr scorermesh_options_extend
                    { if(execute) Parser::Instance()->SetValue<ScorerMesh>((*$1),$3);}
                  | paramassign '=' string scorermesh_options_extend
                    { if(execute) Parser::Instance()->SetValue<ScorerMesh>(*$1,*$3);}


blm_options_extend : /* nothing */
                         | ',' blm_options

blm_options : paramassign '=' aexpr blm_options_extend
                    { if(execute) Parser::Instance()->SetValue<BLMPlacement>((*$1),$3);}
                  | paramassign '=' string blm_options_extend
                    { if(execute) Parser::Instance()->SetValue<BLMPlacement>(*$1,*$3);}

modulator_options_extend : /* nothing */
                         | ',' modulator_options

modulator_options : paramassign '=' aexpr modulator_options_extend
                    { if(execute) Parser::Instance()->SetValue<Modulator>((*$1),$3);}
                  | paramassign '=' string modulator_options_extend
                    { if(execute) Parser::Instance()->SetValue<Modulator>(*$1,*$3);}

query_options_extend : /* nothing */
                     | ',' query_options

query_options : paramassign '=' aexpr query_options_extend
                { if(execute) Parser::Instance()->SetValue<Query>((*$1),$3);}
              | paramassign '=' string query_options_extend
                { if(execute) Parser::Instance()->SetValue<Query>((*$1),*$3);}

colour_options_extend : /* nothing */
                     | ',' colour_options

colour_options : paramassign '=' aexpr colour_options_extend
                { if(execute) Parser::Instance()->SetValue<NewColour>((*$1),$3);}
              | paramassign '=' string colour_options_extend
                { if(execute) Parser::Instance()->SetValue<NewColour>((*$1),*$3);}

crystal_options_extend : /* nothing */
                     | ',' crystal_options

crystal_options : paramassign '=' aexpr crystal_options_extend
                { if(execute) Parser::Instance()->SetValue<Crystal>((*$1),$3);}
              | paramassign '=' string crystal_options_extend
                { if(execute) Parser::Instance()->SetValue<Crystal>((*$1),*$3);}

field_options_extend : /* nothing */
                     | ',' field_options

field_options : paramassign '=' aexpr field_options_extend
                { if(execute) Parser::Instance()->SetValue<Field>((*$1),$3);}
              | paramassign '=' string field_options_extend
                { if(execute) Parser::Instance()->SetValue<Field>((*$1),*$3);}

tunnel_options_extend : /* nothing */
                      | ',' tunnel_options

tunnel_options : paramassign '=' aexpr tunnel_options_extend
                 { if(execute) Parser::Instance()->SetValue<Tunnel>((*$1),$3);}
               | paramassign '=' string tunnel_options_extend
                 { if(execute) Parser::Instance()->SetValue<Tunnel>(*$1,*$3);}

xsecbias_options_extend : /* nothing */
                        | ',' xsecbias_options

xsecbias_options : paramassign '=' aexpr xsecbias_options_extend
                   { if(execute) Parser::Instance()->SetValue<PhysicsBiasing>(*$1,$3);}
                 | paramassign '=' string xsecbias_options_extend
                   { if(execute) Parser::Instance()->SetValue<PhysicsBiasing>(*$1,*$3);}
                 | paramassign '=' vecexpr xsecbias_options_extend
                   { if(execute) Parser::Instance()->SetValue<PhysicsBiasing>(*$1,$3);}

aperture_options_extend : /* nothing */
                         | ',' aperture_options

aperture_options : paramassign '=' aexpr aperture_options_extend
                    { if(execute) Parser::Instance()->SetValue<Aperture>((*$1),$3);}
                  | paramassign '=' string aperture_options_extend
                    { if(execute) Parser::Instance()->SetValue<Aperture>(*$1,*$3);}

option_parameters_extend : /* nothing */
                         | ',' option_parameters

option_parameters : paramassign '=' aexpr option_parameters_extend
                    { if(execute) Parser::Instance()->SetValue<Options>(*$1,$3);}
                  | paramassign '=' string option_parameters_extend
                    { if(execute) Parser::Instance()->SetValue<Options>(*$1,*$3);}

beam_parameters_extend :  /* nothing */
                       | ',' beam_parameters

beam_parameters : paramassign '=' aexpr beam_parameters_extend
                  { if(execute) Parser::Instance()->SetValue<Beam>(*$1,$3);}
                | paramassign '=' string beam_parameters_extend
                  { if(execute) Parser::Instance()->SetValue<Beam>(*$1,*$3);}

extend_options_extend : /* nothing */
                      | ',' extend_options

extend_options : paramassign '=' aexpr extend_options_extend
                 { if(execute) Parser::Instance()->ExtendValue(*($1),$3);}
               | paramassign '=' vecexpr parameters_extend
                 { if(execute) Parser::Instance()->ExtendValue(*($1),$3);}
               | paramassign '=' string beam_parameters_extend
                 { if (execute) Parser::Instance()->ExtendValue(*($1),*$3);}

%%

int yyerror(const char *s)
{
  std::cout << s << " at line " << GMAD::line_num << " of file " << yyfilename << std::endl;
  std::cout << "symbol '" << yytext << "' unexpected (misspelt or semicolon forgotten?)" << std::endl;
  exit(1);
}

int yyerror2(const char *s)
{
  std::cout << s << " at line " << GMAD::line_num << " of file " << yyfilename << std::endl;
  exit(1);
}

extern "C" {
  int yywrap()
  {
    return 1;
  }
}
