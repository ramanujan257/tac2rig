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

[0-9]+  { yylval.num = std::stoi(yytext);
          return num_token; }

[a-z][0-9]* { yylval.str = yytext;
              return id_token;
            }

[:=*+-/<>()\[\]] { return *yytext; }

"goto"  { return goto_token; }

[ \t\n] {}

. {
    std::cerr << "Unknown token: " << *yytext << std::endl;
    exit(EXIT_FAILURE);
}

%%
