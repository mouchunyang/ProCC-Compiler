%{
#include "Node.h"
#include <iostream>
#include <string>
extern int yylex();
extern void yyerror(const char* s);

// Global for the program
NProgram* gProgram = nullptr;

// Disable the warnings that Bison creates
#pragma warning(disable: 4065)
%}

/* You should not need to change the union */
%union {
    Node* node;
	NProgram* program;
	NData* data;
	NDecl* decl;
	NBlock* block;
	NStatement* statement;
	NNumeric* numeric;
	NExpr* expr;
	NComparison* comparison;
	std::string* string;
	int token;
}

%error-verbose

/* Tokens/Terminal symbols */
%token <token> TDATA TMAIN TLBRACE TRBRACE TSEMI TLPAREN TRPAREN
%token <token> TLBRACKET TRBRACKET TINC TDEC TEQUALS
%token <token> TADD TSUB TMUL TDIV
%token <token> TLESS TISEQUAL
%token <token> TVAR TARRAY
%token <token> TIF TELSE TWHILE
%token <token> TCOMMA TPENUP TPENDOWN TSETPOS TSETCOLOR TFWD TBACK TROT
%token <string> TINTEGER TIDENTIFIER

/* Types/non-terminal symbols */
%type <program> program
%type <data> data decls
%type <decl> decl;
%type <numeric> numeric
%type <statement> statement
%type <comparison> comparison
%type <expr> expr;
%type <block> block main


/* Operator precedence */
%left TADD TSUB
%left TMUL TDIV

%%

program		: data main 
				{
                    $$ = new NProgram($1, $2);
                    gProgram = $$;
					std::cout << "Program\n";
				}
;

data		: TDATA TLBRACE TRBRACE
				{
                    $$ = new NData();
					std::cout << "Data (no decls)\n";
				}
			| TDATA TLBRACE decls TRBRACE
				{
                    $$ = $3;
					std::cout << "Data\n";
				}

;

decls		: decl 
				{
                    $$ = new NData();
                    $$->AddDecl($1);
					std::cout << "Single decl\n";
				}
			| decls decl
				{
                    $$->AddDecl($2);
					std::cout << "Multiple decls\n";
				}
;

decl		: TVAR TIDENTIFIER TSEMI
				{
                    $$ = new NVarDecl(*($2));
					std::cout << "Var declaration " << *($2) << '\n';
				}
			| TARRAY TIDENTIFIER TLBRACKET numeric TRBRACKET TSEMI
				{
                    $$ = new NArrayDecl(*($2), $4);
					std::cout << "Array declaration " << *($2) << '\n';
				}
           
;

block       : statement
                {
                    $$ = new NBlock();
                    $$->AddStatement($1);
                    std::cout << "Single Statement\n";
                }
            | block statement
                {
                    $$->AddStatement($2);
                    std::cout << "Multiple Statements\n";
                }
            
;

statement   : TIDENTIFIER TEQUALS expr TSEMI
                {
                    $$ = new NAssignVarStmt(*($1), $3);
                    std::cout << "Vac declaration and assignment " << *($1)
                    <<'\n';
                }
            | TIDENTIFIER TLBRACKET expr TRBRACKET TEQUALS expr TSEMI
                {
                    $$ = new NAssignArrayStmt(*($1), $3, $6);
                    std::cout << "Array Index assignment "
                    <<'\n';
                }
            | TINC TIDENTIFIER TSEMI
                {
                    $$ = new NIncStmt(*($2));
                    std::cout << "Decrement Statement" << std::endl;
                }
            | TDEC TIDENTIFIER TSEMI
                {
                    $$ = new NDecStmt(*($2));
                    std::cout << "Increment Statement" << std::endl;
                }
            | TIF comparison TLBRACE block TRBRACE
                {
                    $$ = new NIfStmt($2, $4, NULL);
                    std::cout << "If statement without else" << std::endl;
                }
            | TIF comparison TLBRACE block TRBRACE TELSE TLBRACE block TRBRACE
                {
                    $$ = new NIfStmt($2, $4, $8);
                    std::cout << "If statement with else" << std::endl;
                }
            | TWHILE comparison TLBRACE block TRBRACE
                {
                    $$ = new NWhileStmt($2, $4);
                    std::cout << "While Loop" << std::endl;
                }
            | TPENUP TLPAREN TRPAREN TSEMI
                {
                    $$ = new NPenUpStmt();
                    std::cout << "While Loop" << std::endl;
                }
            | TPENDOWN TLPAREN TRPAREN TSEMI
                {
                    $$ = new NPenDownStmt();
                    std::cout << "While Loop" << std::endl;
                }
            | TSETPOS TLPAREN expr TCOMMA expr TRPAREN TSEMI
                {
                    $$ = new NSetPosStmt($3, $5);
                    std::cout << "While Loop" << std::endl;
                }
            | TSETCOLOR TLPAREN expr TRPAREN TSEMI
                {
                    $$ = new NSetColorStmt($3);
                    std::cout << "equal to " << '\n';
                }
            | TFWD TLPAREN expr TRPAREN TSEMI
                {
                    $$ = new NFwdStmt($3);
                    std::cout << "equal to " << '\n';
                }
            | TBACK TLPAREN expr TRPAREN TSEMI
                {
                    $$ = new NBackStmt($3);
                    std::cout << "equal to " << '\n';
                }
            | TROT TLPAREN expr TRPAREN TSEMI
                {
                    $$ = new NRotStmt($3);
                    std::cout << "equal to " << '\n';
                }
;

main		: TMAIN TLBRACE TRBRACE
				{
                    $$ = new NBlock();
					std::cout << "Main (no stmts)\n";
				}
            | TMAIN TLBRACE block TRBRACE
                {
                    $$ = $3;
                    std::cout << "Main with stmt\n";
                }
;

expr		: numeric
				{
                    $$ = new NNumericExpr($1);
					std::cout << "Numeric expression\n";
				}
            | expr TADD expr
                {
                    $$ = new NBinaryExpr($1, $2, $3);
                    std::cout << "Add expression\n";
                }
            | expr TSUB expr
                {
                    $$ = new NBinaryExpr($1, $2, $3);
                    std::cout << "Sub expression\n";
                }
            | expr TMUL expr
                {
                    $$ = new NBinaryExpr($1, $2, $3);
                    std::cout << "Mul expression\n";
                }
            | expr TDIV expr
                {
                    $$ = new NBinaryExpr($1, $2, $3);
                    std::cout << "Div expression\n";
                }
            | TLPAREN expr TRPAREN
                {
                    $$ = $2;
                    std::cout << "parenthesis expression\n";
                }
            | TIDENTIFIER
                {
                    $$ = new NVarExpr(*($1));
                    std::cout << "single identifier\n";
                }
            | TIDENTIFIER TLBRACKET expr TRBRACKET
                {
                    $$ = new NArrayExpr(*($1), $3);
                    std::cout << "Array Index accessment "
                    <<'\n';
                }
;

numeric		: TINTEGER
				{
                    $$ = new NNumeric(*($1));
					std::cout << "Numeric value of " << *($1) << '\n';
				}
;

comparison  : expr TLESS expr
                {
                    $$ = new NComparison($1, $2, $3);
                    std::cout << "Less than " << '\n';
                }
            | expr TISEQUAL expr
                {
                    $$ = new NComparison($1, $2, $3);
                    std::cout << "equal to " << '\n';
                }


%%
