#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <regex>
#include "parser.cpp"
#include "lex_2_DFA.cpp"
using namespace std;

map<string,string> alias;//别名
vector<pair<string, string> > regex_rules;//正则表达式--对应的动作
map<string,int> keywords;//关键字
map<int,string> keywords_action;//关键字对应的动作
int keyword_id=0;
//args[1]为文件路径,args[2]为生成的词法分析程序的路径
int main(int argc,char* args[]) {

    ifstream file;
    file.open("c99.l",ios::in);
    ofstream ofs;
    ofs.open("output.cpp",ios::out);

    assert(file.is_open());
    paser(move(read(file)));
    s1_alias(alias);
    //////////
    s2_output(ofs);
    s4_output(ofs);
    /////////
    parser_rules(regex_rules);

    vector<Node_closure*> vec;//NFA数组
    for(auto &e:regex_rules){
        //为关键字
        if(e.first[0]=='\"'&&e.first[e.first.length()-1]=='\"'){
            //cout<<e.first.substr(1,e.first.length()-2)<<endl;
            keywords[e.first.substr(1,e.first.length()-2)]=++keyword_id;
            keywords_action[keyword_id]=e.second;
        }else{
            //cout<<e.first<<"---------\n"<<e.second<<"+++++++++++++\n"<<endl;
            vec.push_back(lex_2_NFA(lex_2_normal(e.first,alias),e.second));
        }
    }
    //////////////////////////////////////
    ofs<<string_init(NFA_2_DFA(union_NFA(vec)),keywords)<<build_yylex(keywords,keywords_action);   
    ///////////////////////////////// 

    file.close();
    ofs.close();
    return 0;
}