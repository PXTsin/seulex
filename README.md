2023年SEU编译原理实践作业

**Lex部分主要工作**

1. 输入文件的解析（Lex RE => 常规RE）
2. 正规式的解析（转语法树或后缀表达）
3. 将每个正规式转换到一个NFA
4. 将所有正规式对应的NFA合并为一个大的NFA
5. NFA的确定化和最小化算法（待实现）实现测试
6. 代码生成

**输入文件的解析**

Lex文件的一般格式为

```
%{
declarations
%}
{definitions}
%%
{rules}
%%
{user subroutines}
```

declarations部分用于定义一些C++代码，它们会被原封不动地copy到词法分析程序中，这部分是可选的。definition部分相当于别名。rules部分用于定义状态机，这部分是最重要的。user subroutines部分用于定义一些C++函数以便简化代码，这部分是可选的。

**Lex RE => 常规RE**

先区分是正规式还是关键字，如“int”为关键字，int前后带双引号“，表示关键字int。
对于正规式，先替换别名，即definitions所定义的，然后处理转义字符。

**正规式转NFA**

```cpp
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
```

![](https://cdn.nlark.com/yuque/0/2023/jpeg/29758174/1684929490662-d9ab3e83-434b-4dc4-9b5c-eb11017dce5b.jpeg)
![](https://cdn.nlark.com/yuque/0/2023/jpeg/29758174/1684929288283-a7998873-cb3c-44df-8771-97ea62cbe315.jpeg)
按顺序读取正规式，依次处理字符

- 碰到 '('、'[' 、'|' 时将字符入栈op，'[' 时还会将一个空的NFA入栈op_num（用作标记）
- 碰到‘)’时处理op栈中的字符‘|’,具体为从op_num中弹出两个NFA，将这两个NFA合成一个大的NFA，入栈op_num，重复处理‘|’直到碰到‘(’,并将‘(’出栈op
- 碰到‘]’时，将op_num中的NFA出栈直到遇到空的NFA，将这些被弹出的NFA合成一个大的NFA，入栈op_num，并将op栈中的‘[’（此时为op栈的栈顶）弹出
- 碰到‘-’时，若在[ ]内，则读取下一个字符m，op_num出栈一个NFA，获取该NFA对应的字符n，创建n到m共（m-n+1）个node节点，并合并成一个NFA，入栈op_num
- 碰到‘*’、‘+’、‘?’时，将op_num出栈一个NFA，对该NFA进行修改形成一个新的NFA，入栈op_num
- 碰到非以上字符时，将其包装成NFA，并入栈op_num

最后将op_num栈内元素依次出栈，连接成一个NFA
![](https://cdn.nlark.com/yuque/0/2023/jpeg/29758174/1684929771229-0eb97fee-24e3-4334-8260-3133e8643e12.jpeg)
![](https://cdn.nlark.com/yuque/0/2023/jpeg/29758174/1684930694430-fe0b6a99-21f9-4398-a850-265cc698fd10.jpeg)
**多个NFA合并成一个大的NFA**
![](https://cdn.nlark.com/yuque/0/2023/jpeg/29758174/1683213603131-9bf7a957-884d-41c9-b52a-455f536f8ec8.jpeg)
**NFA转DFA**
![image.png](https://cdn.nlark.com/yuque/0/2023/png/29758174/1685154570283-aea27201-e712-4faf-80fe-884766c77470.png#averageHue=%23edecec&clientId=u7d3c6fba-5679-4&from=paste&height=585&id=ucef4477e&originHeight=585&originWidth=1044&originalType=binary&ratio=1&rotation=0&showTitle=false&size=159946&status=done&style=none&taskId=ub53f768b-62f7-4528-a0f3-a035b1f98f6&title=&width=1044)
![image.png](https://cdn.nlark.com/yuque/0/2023/png/29758174/1685154581991-334ec04d-8faf-4a03-b369-9793cd0a3cc7.png#averageHue=%23f6f5f5&clientId=u7d3c6fba-5679-4&from=paste&height=535&id=ud7cef6c7&originHeight=535&originWidth=1028&originalType=binary&ratio=1&rotation=0&showTitle=false&size=96250&status=done&style=none&taskId=uad8339f2-448c-4cb4-9eff-6fa15aa11ae&title=&width=1028)
**DFA的最小化（待完成）**
