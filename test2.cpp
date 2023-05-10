/*将正则表达式转化为NFA，再转为DFA*/
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

static int node_id=0;
struct Node;
static map<int,Node*> node_map;//方便debug
static map<int,string> state_action;//map<int,string> state_action 接收状态对应的动作
// NFA 节点结构体
struct Node {
    int id;
    string label;  // 到达该状态的条件
    string action=""; //该状态为接收状态时对应的动作
    bool is_end_state=false;
    vector<Node*> next;  // 转移状态
    Node(string label) : label(label) {
        id=++node_id;
        node_map[id]=this;
    }
};
struct Node_closure{
    Node *head,*tail;
    Node_closure(Node *h,Node *t):head(h),tail(t){}
};
string lex_2_normal(const string lex,map<string,string> &alias){
    string str=lex;
    //去除双引号
    int posi=str.find('\"');
    while(posi>=0){
        if(posi==0||str[posi-1]!='\\'){
            str.erase(str.begin()+posi);
        }
        posi=str.find('\"');
    }
    auto make_string=[](string temp){return "{"+temp+"}";};
    //替换别名
    for(auto &e:alias){
        string tmp=make_string(e.first);
        int position=str.find(tmp);
        while(position!=-1){
            str.replace(position,tmp.length(),e.second);
            position=str.find(tmp);
        }
    }
    //处理转义字符
    for(int i=0;i<str.length();++i){
        // 注意 '\\' 代表 '\' !
        if(str[i]=='\\'&&i+1<str.length()){
            switch (str[i+1]) {
                case '\\':{
                    str.replace(i,2,"\\");
                    break;
                }
                case 'n':{
                    str.replace(i,2,"\n");
                    break;
                }
                case 't':{
                    str.replace(i,2,"\t");
                    break;
                }
                case 'd':{
                    str.replace(i,2,"[0-9]");
                    break;
                }
                default :{
                    str.erase(str.begin()+i);
                    break;
                }
            }
        }
    }
    return str;
}
Node_closure* lex_2_NFA(string lex,string action){
    map<char,int> priority;
    int len=lex.length();
    stack<char> op;
    stack<Node_closure*> op_num;
    // - |  ( ) [ ] * +  ?
    // - | 双目运算符
    //* + ? 单目运算符直接算
    for(int i=0;i<len;++i){
        switch(lex[i]){
            case '(':{
                op.push('(');
                break;
            }
            case ')':{
                char oper=op.top();op.pop();
                if(oper!='('){
                    switch(oper){
                        case '|':{
                            Node_closure *t1=op_num.top();op_num.pop();
                            Node_closure *t2=op_num.top();op_num.pop();
                            Node *head(new Node("")),*tail(new Node(""));
                            head->next.push_back(t1->head);
                            head->next.push_back(t2->head);
                            t1->tail->next.push_back(tail);
                            t2->tail->next.push_back(tail);
                            op_num.push(new Node_closure(head,tail));
                            break;
                        }
                        case '-':{
                            Node_closure *t1=op_num.top();op_num.pop();
                            Node_closure *t2=op_num.top();op_num.pop();
                            Node *head(new Node("")),*tail(new Node(""));
                            char c2=t1->tail->label[0];
                            char c1=t2->tail->label[0];
                            while(c1<=c2){
                                string str;str=c1;
                                Node *t_head=new Node(str);
                                Node *t_tail=new Node("");
                                t_tail->next.push_back(t_head);
                                head->next.push_back(t_tail);
                                t_head->next.push_back(tail);
                                ++c1;
                            }
                            op_num.push(new Node_closure(head,tail));
                        }
                    }
                    oper=op.top();op.pop();//弹出'('
                }
                break;
            }
            case '[':{
                op_num.push(new Node_closure(nullptr,new Node("")));//前空表示'['
                op.push('[');
                break;
            }
            case ']':{
                char oper=op.top();op.pop();
                vector<Node_closure*> vec_temp;
                while(oper!='['){
                    switch(oper){
                        case '-':{
                            Node_closure *t1=op_num.top();op_num.pop();
                            Node_closure *t2=op_num.top();op_num.pop();
                            Node *head(new Node("")),*tail(new Node(""));
                            char c2=t1->tail->label[0];
                            char c1=t2->tail->label[0];
                            while(c1<=c2){
                                string str;str=c1;
                                Node *t_head=new Node(str);
                                Node *t_tail=new Node("");
                                t_tail->next.push_back(t_head);
                                head->next.push_back(t_tail);
                                t_head->next.push_back(tail);
                                ++c1;
                            }
                            vec_temp.push_back(new Node_closure(head,tail));
                            break;
                        }
                        default:break;
                    }
                    oper=op.top();op.pop();//弹出'['
                }
                Node_closure *temp=op_num.top();op_num.pop();
                while (temp->head!=nullptr)//遇到op_num里的'['则停止循环
                {
                    vec_temp.push_back(temp);
                    temp=op_num.top();op_num.pop();
                }
                Node *head(new Node("")),*tail(new Node(""));
                for(auto &e:vec_temp){
                    head->next.push_back(e->head);
                    e->tail->next.push_back(tail);
                }
                op_num.push(new Node_closure(head,tail));
                break;
            }
            case '-':{
                op.push('-');
                break;
            }
            case '|':{
                op.push('|');
                break;
            }
            case '*':{
                Node *head=new Node("");
                Node *tail=new Node("");
                head->next.push_back(tail);
                Node_closure *temp=op_num.top();op_num.pop();
                head->next.push_back(temp->head);
                temp->tail->next.push_back(tail);
                tail->next.push_back(temp->head);
                op_num.push(new Node_closure(head,tail));
                break;
            }
            case '?':{
                Node *head=new Node("");
                Node *tail=new Node("");
                head->next.push_back(tail);
                Node_closure *temp=op_num.top();op_num.pop();
                head->next.push_back(temp->head);
                temp->tail->next.push_back(tail);
                op_num.push(new Node_closure(head,tail));
                break;
            }
            case '+':{
                Node *head=new Node("");
                Node *tail=new Node("");
                tail->next.push_back(head);
                Node_closure *temp=op_num.top();op_num.pop();
                head->next.push_back(temp->head);
                temp->tail->next.push_back(tail);
                op_num.push(new Node_closure(head,tail));
                break;
            }
            default:{
                Node *head=new Node("");
                string str;
                str=lex[i];
                Node *tail=new Node(str);
                head->next.push_back(tail);
                op_num.push(new Node_closure(head,tail));
                break;
            }
        }
    }
    while(op_num.size()>1){
        Node_closure *n1=op_num.top();op_num.pop();//tail
        Node_closure *n2=op_num.top();op_num.pop();//head
        n2->tail->next.push_back(n1->head);
        op_num.push(new Node_closure(n2->head,n1->tail));
    }
    op_num.top()->tail->is_end_state=true;
    op_num.top()->tail->action=action;
    return op_num.top();
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


vector<vector<short>> NFA_2_DFA(Node* root){
    int vec_size=512;
    vector<vector<short>> vec(vec_size,vector<short>(127,0));
    map<vector<Node*>,short> my_map;
    vector<Node*> temp=closure_ε(root);
    short id=0,count=1;
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
                                state_action[abs(my_map[tmp])]=e->action;
                            }
                        }
                        /*--------------*/
                    }  
                }
                --count;//未标记状态-1
            }
        }
    }
    int num=vec_size;
    for(auto &tmp:vec){
        int n=accumulate(tmp.begin(),tmp.end(),0);
        if(n==0)
            --num;
    }
    vector<vector<short>> ans;
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
            if(p->label == e->label&&p->label!=""){
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
/*定义状态转移矩阵*/
string string_matrix(vector<vector<short>> ans){
    int ans_size=ans.size();
    string str="    m=new vector<vector<short>>("+to_string(ans_size)+",vector<short>(127,0));\n";
    for(int i=0;i<ans_size;++i){
        for(int j=0;j<127;++j){
            if(ans[i][j]!=0){
                str+="    m[" + to_string(i) + "][" + to_string(j)+ "]=" + to_string(ans[i][j]) + ";\n";
            }
        }
    }
    return str;
}
/*定义结束状态以及对应的动作*/
string string_end_state(map<int,string> state_action){
    string str="\n";
    for(auto &e:state_action){
        str+="    state_action["+to_string(e.first)+"]="+e.second+";\n";
    }
    return str;
}
/*定义switch程序，执行对应的动作*/
string string_switch_program(map<int,string> action){
    string str="    auto switch_program=[](int state){\n";
    for(auto &e:action){
        str+="\tcase "+to_string(e.first)+" :{\n    \t"+e.second+"\n    \tbreak;\n\t}\n";
    }
    str+="\tdefault:return (ERROR);\n    }\n    int ans=switch_program(state);\n    return ans;\n";
    return str;
}
string build_yylex(map<int,string> action){
    string body="void yylex(){\n\
    if(buffer.empty()){\n\
        stringstream ss;\n\
        string str;\n\
        ++lineno;\n\
        fin>>ss;\n\
        while(ss>>str){\n\
            buffer.push(str);\n\
        }\n\
    }\n\
    string str=buffer.front();buffer.pop();\n\
    current=str;\n\
    int i=0;\n\
    int state=1;\n\
        while(state_action.count(state)==0){\n\
        char k=str[i++];\n\
        state=m[state][k];\n\
    }\n"+string_switch_program(action)+"}\n";
    return body;
}
/*定义全局变量*/
string global_var(){
    string str="\
    fstream fin;\n\
    map<int,string> state_action;\n\
    vector<vector<short>> m;\n\
    queue<string> buffer;\n\
    int lineno=0;\n\
    int current=0;\n";
    return str;
}
/*定义初始化状态转移矩阵和接收状态集合*/
string string_init(vector<vector<short>> vec,map<int,string> state){
    string str="void init(){\n"+string_matrix(vec)+string_end_state(state)+"}";
    return str;
}
void test_lex_2_normal(){
    map<string,string> alias;
    alias["D"]="[0-9]";
    alias["L"]="[_a-zA-Z]";
    string str=lex_2_normal("\\d",alias);//\"//\"(.*)(\n)?
    Node_closure *n_c=lex_2_NFA(str,"test");
    cout<<str<<endl;
    vector<vector<short>> vec=NFA_2_DFA(n_c->head);
}
int main(){ 
    //string str="(a|b)c[1-3]+";//[a-zA-Z_]([a-zA-Z_]|[0-9])*  [a-zA-Z_]
    //string str="[_a-zA-Z]([_a-zA-Z]|[0-9])*";
    //Node_closure *n_c=lex_2_NFA(str,"test");
    //Node *p=n_c->head;
    //my_print(p,0);
    //vector<vector<short>> vec=NFA_2_DFA(p);
    //for(auto e:state_action){
    //    cout<<"接收状态"<<e.first<<"对应的动作为:\n"<<e.second<<endl;
    //}
    //cout<<string_matrix(vec)<<endl;
    //cout<<string_end_state(state_action)<<endl;
    //cout<<string_switch_program(state_action)<<endl;
    //cout<<build_yylex(state_action)<<endl;
    //cout<<string_init(vec,state_action)<<endl;

    test_lex_2_normal();
    return 0;
}
