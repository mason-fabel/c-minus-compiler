%{
extern int yylex(void);
extern void yyerror(const char*);
%}

%token A
%token B
%token LPAREN
%token RPAREN

%start expr

%%

expr : expr expr | A | B | /* empty */;

%%
