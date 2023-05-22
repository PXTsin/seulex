D			[0-9]
L			[a-zA-Z_]

%{
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <unordered_map>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <numeric>
#include <queue>
using namespace std;

#define WHITESPACE 0  
 /*sp */ 

#define ADD_ASSIGN 318
#define AND_ASSIGN 322
#define AND_OP 313
#define AUTO 329
#define BOOL 344
#define BREAK 361
#define CASE 351
#define CHAR 333
#define COMPLEX 345
#define CONST 341
#define CONSTANT 301
#define CONTINUE 360
#define DEC_OP 306
#define DEFAULT 352
#define DIV_ASSIGN 316
#define DO 357
#define DOUBLE 340
#define ELLIPSIS 350
#define ELSE 354
#define ENUM 349
#define EQ_OP 311
#define EXTERN 327
#define FLOAT 339
#define FOR 358
#define GE_OP 310
#define GOTO 359
#define IDENTIFIER 300
#define IF 353
#define IMAGINARY 346
#define INC_OP 305
#define INLINE 331
#define INT 335
#define LEFT_ASSIGN 320
#define LEFT_OP 307
#define LE_OP 309
#define LONG 336
#define MOD_ASSIGN 317
#define MUL_ASSIGN 315
#define NE_OP 312
#define OR_ASSIGN 324
#define OR_OP 314
#define PTR_OP 304
#define REGISTER 330
#define RESTRICT 332
#define RETURN 362
#define RIGHT_ASSIGN 321
#define RIGHT_OP 308
#define SHORT 334
#define SIGNED 337
#define SIZEOF 303
#define STATIC 328
#define STRING_LITERAL 302
#define STRUCT 347
#define SUB_ASSIGN 319
#define SWITCH 355
#define TYPEDEF 326
#define TYPE_NAME 325
#define UNION 348
#define UNSIGNED 338
#define VOID 343
#define VOLATILE 342
#define WHILE 356
#define XOR_ASSIGN 323
void init();
void count(){}
int yylex();
fstream fin;
map<string,int> keywords;
map<int,string> state_action;
vector<vector<short>> m;
queue<string> buffer;
int lineno=0;
int colno=0;
string current;
%}

%%
\"[!#-~]*\" { return (STRING_LITERAL); }
{L}         { return(IDENTIFIER); }
{L}({L}|{D})*		{ return(IDENTIFIER); }
[1-9]{D}*						{ return(CONSTANT); }
{D}+								{ return(CONSTANT); }
{D}*"."{D}*					{ return(CONSTANT); }
{D}+"."{D}*					{ return(CONSTANT); }
[\n\r\t] { return (WHITESPACE); }
"auto"			{ count(); return(AUTO); }
"_Bool"			{ count(); return(BOOL); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"_Complex"		{ count(); return(COMPLEX); }
"const"			{ count(); return(CONST); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"extern"		{ count(); return(EXTERN); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"_Imaginary"		{ count(); return(IMAGINARY); }
"inline"		{ count(); return(INLINE); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"register"		{ count(); return(REGISTER); }
"restrict"		{ count(); return(RESTRICT); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"static"		{ count(); return(STATIC); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"volatile"		{ count(); return(VOLATILE); }
"while"			{ count(); return(WHILE); }
"..."			{ count(); return(ELLIPSIS); }
">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }
"{"         { count(); return('{'); }
"}"		    { count(); return('}'); }
"["         { count(); return('['); }
"]"         { count(); return(']'); }

%%

int yywrap() {
  return 1;
}
int main(int argc,char *argv[]){
    string filename;
    if(argc<2){
        filename="test.txt";
    }else{
        filename=argv[1];
    }
    fin.open(filename,ios::in);
    init();
	//////////
    int c=0;
	while((c=yylex())!=0){
        cout<<c<<" < "<<lineno<<" , "<<colno<<" > "<<current<<"\x06\n";
    }
    ////////////
    fin.close();
    return 0;
}