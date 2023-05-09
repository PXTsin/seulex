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
    Node(string label) : label(label) {
        id=++node_id;
        node_map[id]=this;
    }
};
struct Node_closure{
    Node *head,*tail;
    Node_closure(Node *h,Node *t):head(h),tail(t){}
};
Node_closure* lex_2_NFA(string lex){
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
                tail->next.push_back(head);
                Node_closure *temp=op_num.top();op_num.pop();
                head->next.push_back(temp->head);
                temp->tail->next.push_back(tail);
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
    return op_num.top();
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
int main(){
    string str="abc[1-3]+";
    Node *p=lex_2_NFA(str)->head;
    //my_print(p,0);
    NFA_2_DFA(p);
    return 0;
}
