/*将正则表达式转化为NFA*/
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <unordered_map>
#include <regex>
#include <map>
#include <set>
#include <numeric>
using namespace std;

static int node_id=0;
struct Node;
static map<int,Node*> node_map;
// NFA 节点结构体
struct Node {
    int id;
    string label;  // 节点标签标识
    bool is_end_state=false;
    vector<Node*> next;  // 转移状态
    Node(){

    }
    Node(string label) : label(label) {
        id=++node_id;
        node_map[id]=this;
    }
};

/* 1.Lex RE=> state */
vector<pair<string,int>> lex_2_normal(const string lex){
    vector<pair<string,int>> temp;//RE对应的状态
    int len=lex.length();
    int mid_num=0;// [ 减去 ] 的数量
    int small_num=0;//( 减去 ) 的数量
    string str="";
    for(int i=0;i<len;++i){
        // 注意 '\\' 代表 '\' !
        if(lex[i]=='\\'&&i+1<len){
            switch (lex[++i]) {
                case '\\':{
                    str+="\\|";
                    break;
                }
                case 'd':{// '\d'等价于 [0-9]
                    str+="0|1|2|3|4|5|6|7|8|9";
                    break;
                }
                default:{//其它功能暂不支持,原样返回
                    if(str.length())
                        str.push_back('|');
                    str.push_back(lex[i]);
                    break;
                }
            }
        }
        else{
            switch (lex[i]) {
                case '[':{
                    ++mid_num;
                    str="";
                    break;
                }
                case ']':{
                    --mid_num;
                    temp.push_back(pair<string,int>(str,0));
                    break;
                }
                case '-':{//范围匹配
                    if(i-1>=0&&i+1<len){
                        char prev=lex[i-1];
                        char next=lex[++i];
                        while(prev<next){
                            //因为之前就入栈了，所以直接从下一个字符开始
                            ++prev;
                            str.push_back('|');
                            str.push_back(prev);
                        }
                    }
                    break;
                }
                case '|':{
                    break;
                }
                case '(':{

                    break;
                }
                case ')':{

                    break;
                }
                    //? + * .
                case '?':{// 1
                    temp.back().second=1;
                    break;
                }
                case '+':{// 2
                    temp.back().second=2;
                    break;
                }
                case '*':{// 3
                    temp.back().second=3;
                    break;
                }
                case '.':{// 4
                    temp.back().second=4;
                    break;
                }
                default:{
                    if(mid_num>0){
                        if(str.length())
                            str.push_back('|');
                        str.push_back(lex[i]);
                    }else{
                        str="";
                        str.push_back(lex[i]);
                        temp.push_back(pair<string,int>(str,0));
                        str="";
                    }
                    break;
                }
                /*case '(' ')' '{' '}'*/
            }
        }
    }
    for(auto &e:temp){
        cout<<e.first<<"\ttype:"<<e.second<<endl;
    }
    return temp;
}
void split(const string& s,vector<string>& sv,const char flag = ' ') {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
    return;
}
Node* build_NFA(vector<pair<string,int>> vec,string action){
    if(vec.empty()){
        return nullptr;
    }
    int len=vec.size();
    Node *head=new Node("");
    Node *p=head;
    Node *prev;
    for(int i=0;i<len;++i){
        //建立表示空的节点
        Node *emp=new Node("");

        vector<string> str_vec;
        vector<Node*> node_vec;
        split(vec[i].first,str_vec,'|');
        for(auto str:str_vec){
            node_vec.emplace_back(new Node(str));
            node_vec.back()->next.push_back(emp);
        }

        //连接上一个状态与当前状态
        if(i>0){
            p=prev;
        }
        switch (vec[i].second) {
            case 0:{
                for(auto &ttt:node_vec){
                    p->next.push_back(ttt);
                }
                break;
            }
            case 1:{// ? 0或1次
                for(auto &ttt:node_vec){
                    p->next.push_back(ttt);
                }

                p->next.push_back(new Node(""));
                break;
            }
            case 2:{// + 1或多次
                for(auto &ttt:node_vec){
                    p->next.push_back(ttt);
                }
                emp->next.push_back(p);
                break;
            }
            case 3:{// 0、1或多次
                p->next.push_back(new Node(""));
                for(auto &ttt:node_vec){
                    p->next.push_back(ttt);
                }
                emp->next.push_back(p);
            }
            default:{
                for(auto &ttt:node_vec){
                    p->next.push_back(ttt);
                }
                break;
            }
        }
        prev=emp;
    }
    Node *tail=new Node("");
    tail->is_end_state=true;
    prev->next.push_back(tail);
    return head;
}
Node* union_NFA(vector<Node*> vec){
    Node *start=new Node("");
    for(auto &e:vec){
        start->next.push_back(e);
    }
    return start;
}
/*求空闭包,是从状态t只经过ε转换可以到达的状态集*/
vector<Node*> closure_ε(Node *t){
    vector<Node*> ans;
    ans.push_back(t);
    auto &vec=t->next;
    for(auto &e:vec){
        if(e->label==""){
            auto vec2=closure_ε(e);
            ans.push_back(e);
            for(auto &a:vec2){
                ans.push_back(a);
            }
        }
    }
    return ans;
}
/*closure_ε(T)是从T中的任一状态只经过ε转换可以到达的状态集*/
vector<Node*> closure_ε(vector<Node*> T){
    vector<Node*> ans;
    set<int> s;//去重
    for(auto &e:T){
        vector<Node*> temp=closure_ε(e);
        for(auto &a:temp){
            if(s.count(a->id)==0){
                ans.push_back(a);
                s.insert(a->id);
            }
        }
    }
    return ans;
}
/*从状态t经过a边所能到达的状态集*/
vector<Node*> move(Node *t,char a){
    string s;s=a;
    vector<Node*> vec=closure_ε(t);
    vector<Node*> ans;
    vec.push_back(t);
    for(auto &e:vec){
        auto temp=e->next;
        for(auto &tmp:temp){
            if(tmp->label==s){
                ans.push_back(tmp);
            }
        }
    }
    return ans;
}
/*从T中的状态经过非空的a转换可以到达的状态集*/
vector<Node*> move(vector<Node*> T,char a){
    vector<Node*> ans;
    set<int> s;//去重
    for(auto &t:T){
        auto temp=move(t,a);
        for(auto &e:temp){
            if(s.count(e->id)==0){
                s.insert(e->id);
                ans.push_back(e);
            }
        }
    }
    return ans;
}
vector<vector<int>> NFA_2_DFA(Node* root){
    vector<vector<int>> vec(128,vector<int>(127,0));
    map<vector<Node*>,int> my_map;
    vector<Node*> temp=closure_ε(root);
    set<int> end_state;//结束状态集合
    int id=0,count=1;
    my_map[temp]=-(++id);
    while(count>0){
        for(auto &my_pair:my_map){
            if(my_pair.second<0){//没有标记
                my_pair.second=abs(my_pair.second);//加上标记
                for(char i=0;i<=126;++i){
                    vector<Node*> tmp=closure_ε(move(my_pair.first,i));
                    if(!tmp.empty()){
                        if(!my_map[tmp]){
                            my_map[tmp]=-(++id);
                            ++count;//未标记状态+1
                        }
                        vec[abs(my_map[my_pair.first])][i]=abs(my_map[tmp]);//建立状态转移
                        /*--------------*/
                        for(auto &e:tmp){
                            if(e->is_end_state){
                                end_state.insert(abs(my_map[tmp]));
                            }
                        }
                        /*--------------*/
                    }  
                }
                --count;//未标记状态-1
            }
        }
    }
    int num=128;
    for(auto &tmp:vec){
        int n=accumulate(tmp.begin(),tmp.end(),0);
        if(n==0)
            --num;
    }
    vector<vector<int>> ans;
    for(int i=0;i<=num;++i){
        ans.push_back(vec[i]);
    }
    //打印状态转移矩阵
    for(int i=0;i<ans.size();++i){
        for(int j=0;j<ans[i].size();++j){
            if(ans[i][j]!=0)
                cout<<i<<":-"<<(char)j<<"->"<<ans[i][j]<<endl;
        }
    }
    //////////////////////////////////
    for(auto e:end_state){
        cout<<e<<endl;
    }
    return ans;
}
void my_print(Node *p,int n){
    string str="";
    for(int i=0;i<n;++i){
        str+="    ";
    }
    if (p!= nullptr){

        cout<<str<<"state:"<<p->id<<endl;
        cout<<str<<"condition:"<<p->label<<endl;
        cout<<str<<"next:{"<<endl;
        for(auto &e:p->next){
            if(p->label == e->label){
                cout<<str+"    "<<"state:"<<e->id<<endl; 
                cout<<str+"    "<<"condition:"<<e->label<<endl;
                cout<<str+"    "<<"next:{loop!}"<<endl;
            }else{
                my_print(e,n+1);
            }
        }
        cout<<str<<"}"<<endl;
    }
}
int main(){
    string str="a[1-3]*";
    Node *p= build_NFA(lex_2_normal(str),"id");
    my_print(p,0);
    NFA_2_DFA(p);
    //move(node_map[2],'1');
    // auto a=move(node_map[1],"a");
    // for(auto e:a){1
    //     cout<<"转移条件:"<<e->label<<"\tid:"<<e->id<<endl;
    // }
    return 0;
}
