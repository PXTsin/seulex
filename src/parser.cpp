#ifndef parser
#define parser

#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <regex>
using namespace std;

string read(ifstream &file){
    string str;
    string temp="";
    while (getline(file,temp))
    {
        str+=temp+"\n";
    }
    return str;
}
string s1,s2,s3,s4;
//string &s1,string &s2,string &s3,
void paser(string src){
    s1.clear();s2.clear();s3.clear(),s4.clear();

    int p1=src.find("%{");
    int p2=src.find("%}");
    s2=src.substr(p1+2,p2-p1-2);
    src.erase(src.begin()+p1,src.begin()+p2+2);

    int p3=src.find("%%");
    s1=src.substr(0,p3);
    src.erase(src.begin(),src.begin()+p3+2);

    int p4=src.find("%%");
    s3=src.substr(0,p4);
    src.erase(src.begin(),src.begin()+p4+2);

    s4=src;
}
//获取别名
void s1_alias(map<string,string> &alias){
    stringstream ss;
    string buffer;
    int pos=s1.find("\n");
    while(pos>=0){
        buffer=s1.substr(0,pos);
        //不为空
        if(!regex_match(buffer, regex("\\s*"))){
            ss.clear();
            ss<<buffer;
            string t1,t2;
            ss>>t1>>t2;
            alias[t1]=t2;
            //cout<<t1<<"----"<<t2<<endl;
        }
        s1.erase(s1.begin(),s1.begin()+pos+1);
        pos=s1.find("\n");
    }
}
//原样输出
void s4_output(ofstream &ofs){
    ofs<<s4;
}
void s2_output(ofstream &ofs){
    ofs<<s2;
}
//获取正则表达式--对应的动作
void parser_rules(vector<pair<string, string> > &rules){
    stringstream f;
    f<<s3;
    string temp;
    string s,prev="";
    bool comment= false;//是否为注释
    int num=0;//{ 减去 }的个数
    string regex_str;
    
    while (f>>s){
        if(s=="/*"){
            comment= true;
        }
        else if(s=="*/"){
            comment= false;
        }
        else if(s=="{"&&!comment){
            ++num;
            if(num==1){
                regex_str=prev;
                rules.push_back(pair<string, string>(regex_str, ""));
            }
        }
        else if(s=="}"&&!comment){
            --num;
            if(num==0){
                temp+=s+"  ";
                rules[rules.size()-1].second+=temp;
                temp="";
            }
        }
        if(num>0&&!comment)
            temp+=s+"  ";
        prev=s;
    }
    // for(auto &e:rules){
    //     cout<<e.first<<"-----\n"<<e.second<<"++++++++\n";
    // }
}
#endif