%option noinput
%option nounput
%option noyywrap

%{
#include <string>
#include <iostream>
#include <vector>
#include "basic_blocks.hpp"

#include "parser.tab.hpp"
%}

%%
	/* if i goto moramo da hvatamo pre svega, 
	   da ga ne bi slucajno pokupio regex za id_token */

"if"	{ return if_token;	}
"goto"  { return goto_token; }
"return" { return return_token; }

[0-9]+  { yylval.n= std::stoi(yytext);
          return num_token; }

[a-z][0-9]* { yylval.s= new std::string(yytext);
              return id_token;
            }

[:=*+/<>()\[\]\n-] { return *yytext; }


[ \t] {}

\%\%.* {}

. {
    std::cerr << "Unknown token: " << *yytext << std::endl;
    exit(EXIT_FAILURE);
}

%%
