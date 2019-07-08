%option noinput
%option nounput
%option noyywrap

%{
#include <string>
#include <iostream>
#include <vector>

#include "parser.tab.hpp"
%}

%%

[0-9]+  { yylval.num = stoi(yytext);
          return num_token; }

[a-z][0-9]* { yylval.str = yytext;
              return id_token;
            }

[:=*+-/<>()\[\]] { return *yytext; }

"goto"  { return goto_token; }

%%
