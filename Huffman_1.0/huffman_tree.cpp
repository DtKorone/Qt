#include<iostream>
#include<queue>
#include<vector>
#include<cstring>
#include<fstream>
#include<unordered_map>
#include "huffman_tree.h"
using namespace std;
const int SYMBOL_MAX_NUM=256;   //单个字节能表示的最多字符数
//原因是存在值为的负数字符（一个中文字符由两个负数字符组成）
const int DYNAMIC_MAX_NUM=1000000;

unordered_map<int,char*>Huffman_code;
//TreeNode
TreeNode::TreeNode()
{
    symbol=0;
    weight=0;
    lchild=nullptr;
    rchild=nullptr;
}
TreeNode::TreeNode(char s,int w)
{
    symbol=s;
    weight=w;
    lchild=nullptr;
    rchild=nullptr;
}
//Huffman_tree
HuffmanTree::HuffmanTree()
{
    root=nullptr;
}
HuffmanTree::~HuffmanTree()
{
    free_tree(root);
}
bool HuffmanTree::encoding(const char *source_file,const char *target_file)//二进制压缩
{
    char ch;//读入字符
    int i=0;
    size_t j=0;
    char *s_str =new char[DYNAMIC_MAX_NUM]; //读入字符串
    char *t_str = new char[DYNAMIC_MAX_NUM*SYMBOL_MAX_NUM];//编码字符串

    unordered_map<char,int>chs;//记录字符出现频率
    char sym[SYMBOL_MAX_NUM];//记录出现的字符,对于中文字符来说，可能会出现负数，所以可能有两倍的字节数多
    int w[SYMBOL_MAX_NUM];//出现的字符频率
    int len=0;  //出现的字符总数
    //读取文件，获得总字符串
    ifstream infile(source_file);
    if(!infile)
    {
        delete []s_str;//释放数组
        delete []t_str;
        cout<<"Open Source File: "<<source_file<<" Error";
        return false;
    }
    while(infile.get(ch))
    {
        s_str[len]=ch;
        chs[ch]++;
        len++;
    }
    infile.close();

    s_str[len]='\0';

    //获得出现字符和频率
    int sym_num=0;    //出现的字符数

    for(auto temp:chs)
    {
        sym[sym_num]=temp.first;
        w[sym_num]=temp.second;
        sym_num++;
    }
    sym[sym_num]='\0';

    //创建huffman树
    create_tree(sym,w,sym_num);

    //进行编码，编码保存在Huffman_code
    get_code(root,0);

    //对每个字符进行编码
    t_str[0]='\0';
    for(i=0; i<len; i++)
        strcat(t_str,Huffman_code[(int)s_str[i]]);

    //将01字符串转化为二进制输入文件
    size_t t_str_len=strlen(t_str);//目标字符串总长度
    char offset=(8-t_str_len%8);//末尾补零数

    ofstream outfile(target_file,ios::binary);
    if(!outfile)
    {
        delete []s_str;//释放数组
        delete []t_str;
        cout<<"Open Target File: "<<target_file<<" Error";
        return false;
    }
    //写入哈夫曼字符数、字符、字符权重、末尾补零数
    outfile.write((char*)&sym_num,sizeof(int));
    for(i=0; i<sym_num; i++)
        outfile.write(&sym[i],sizeof(char));
    for(i=0; i<sym_num; i++)
        outfile.write((char*)&w[i],sizeof(int));
    outfile.write(&offset,sizeof(char));
    ch=0;
    j=0;
    while(j<t_str_len)
    {
        for(i=0; i<8; i++)
        {
            ch=ch<<1;
            if(i+j<t_str_len)
            {
                if(t_str[i+j]=='0') continue;
                if(t_str[i+j]=='1') ch=ch|1;
            }
        }
        j+=8;
        outfile.write(&ch,sizeof(char));
    }
    outfile.close();
    delete []s_str;//释放数组
    delete []t_str;
    return true;
}
bool HuffmanTree::decoding(const char *source_file,const char *target_file)//二进制解压
{
    size_t i=0;
    size_t j=0,len=0; //len为最终编码长度
    TreeNode *cur;  //当前节点，用于遍历树，进行编码
    char ch;//读入字符
    char *s_str =new char[DYNAMIC_MAX_NUM*SYMBOL_MAX_NUM];//读入字符串
    char *t_str = new char[DYNAMIC_MAX_NUM];//解码字符串
    char offset;//文件末尾补零数
    char temp_code[9];//暂存读取的编码，
    s_str[0]='\0';
    char sym[SYMBOL_MAX_NUM];//记录出现的字符,对于中文字符来说，可能会出现负数，所以可能有两倍的字节数多
    int w[SYMBOL_MAX_NUM];//出现的字符频率
    int sym_num=0;

    char ch0=0x00;//0000 0000   两个用于判断字符的最后一位
    char ch1=0x01;//0000 0001

    //读取文件
    ifstream infile(source_file,ios::binary);
    if(!infile)
    {
        delete []s_str;//释放数组
        delete []t_str;
        cout<<"Open Source File: "<<source_file<<" Error";
        return false;
    }
    //读取哈夫曼字符数、字符、字符权重、读取文件末补0数
    int tempi=0;
    infile.read((char*)&sym_num,sizeof(int));
    for(tempi=0; tempi<sym_num; tempi++)
        infile.read(&sym[tempi],sizeof(char));
    for(tempi=0; tempi<sym_num; tempi++)
        infile.read((char*)&w[tempi],sizeof(int));

    infile.read(&offset,sizeof(char));

    //将读取的二进制码进行转换成01字符串
    while(infile.read(&ch,sizeof(char)))
    {
        for(i=0; i<8; i++)
        {
            if((ch&ch1)==ch0)
            {
                temp_code[8-i-1]='0';
            }
            if((ch&ch1)==ch1)
            {
                temp_code[8-i-1]='1';
            }
            len++;
            ch=ch>>1;
        }
        temp_code[8]='\0';
        strcat(s_str,temp_code);
    }
    infile.close();

    //先释放哈夫曼树
    free_tree(root);
    //重建建哈夫曼树
    create_tree(sym,w,sym_num);

    s_str[len-(int)offset]='\0';//真正文件编码在文件结束前offset处结束

    //进行解码
    i=0;
    while(i<strlen(s_str))
    {
        cur=root;
        while(1)
        {
            if(s_str[i]=='0'&&cur->lchild)
                cur=cur->lchild;
            else if(s_str[i]=='1'&&cur->rchild)
                cur=cur->rchild;
            else break;
            i++;
        }
        t_str[j]=cur->symbol;
        j++;
    }
    t_str[j]='\0';

    //将解码后的字符串写入文件
    ofstream outfile(target_file);
    if(!outfile)
    {
        delete []s_str;//释放数组
        delete []t_str;
        cout<<"Open Target File: "<<target_file<<" Error";
        return false;
    }
    j=0;
    while(t_str[j]!='\0')
    {
        outfile<<t_str[j];
        j++;
    }
    outfile.close();
    delete []s_str;//释放数组
    delete []t_str;
    return true;
}

void HuffmanTree::create_tree(char *sym,int *w,int n)//构造树    如果只有一种字符的话，无法构造哈夫曼树，因为无法进行编码
{
    priority_queue<TreeNode*,vector<TreeNode*>,cmp>nodes;
    TreeNode *node;
    for(int i=0; i<n; i++)
    {
        node=new TreeNode(sym[i],w[i]);
        nodes.push(node);
    }
    TreeNode *lnode,*rnode,*new_node;
    while(nodes.size()>1)
    {
        lnode=nodes.top();
        nodes.pop();
        rnode=nodes.top();
        nodes.pop();
        new_node=new TreeNode(0,lnode->weight+rnode->weight);
        new_node->lchild=lnode;
        new_node->rchild=rnode;
        nodes.push(new_node);
    }
    if(!nodes.empty())  //当文档为空时，则不会创建节点
    {
        root=nodes.top();
        nodes.pop();
    }
}
void HuffmanTree::get_code(TreeNode *node,int len)
{
    static char code[SYMBOL_MAX_NUM]= {'\0'};
    char *sym_code;
    int a;
    if(!node) return;
    if(!node->lchild&&!node->rchild)
    {
        a=int(node->symbol);
        //cout<<node->symbol<<" "<<code<<endl;
        sym_code=new char[SYMBOL_MAX_NUM];
        strcpy(sym_code,code);
        sym_code[len]='\0';
        //cout<<sym_code<<endl;
        Huffman_code[a]=sym_code;
        // cout<<Huffman_code[a]<<endl;
    }
    else
    {
        code[len]='0';
        get_code(node->lchild,len+1);
        code[len]='1';
        get_code(node->rchild,len+1);
    }
}
void HuffmanTree::free_tree(TreeNode *node)//释放树
{
    if(node)
    {
        free_tree(node->lchild);
        free_tree(node->rchild);
        delete node;
        node=nullptr;
    }
}
/*
void HuffmanTree::encoding_to_text(char *source_file,char *target_file)//压缩
{
    char ch;//读入字符
    char *s_str =new char[DYNAMIC_MAX_NUM]; //读入字符串
    char *t_str = new char[DYNAMIC_MAX_NUM];//编码字符串

    unordered_map<char,int>chs;//记录字符出现频率
    char sym[SYMBOL_MAX_NUM];//记录出现的字符,对于中文字符来说，可能会出现负数，所以可能有两倍的字节数多
    int w[SYMBOL_MAX_NUM];//出现的字符频率
    int len=0;
    //读取文件
    ifstream infile(source_file);
    if(!infile)
    {
        cout<<"Open File: "<<source_file<<" Error";
        return ;
    }
    while(infile.get(ch))
    {
        s_str[len]=ch;
        chs[ch]++;
        len++;
    }
    infile.close();

    s_str[len]='\0';
    //得到出现字符和频率
    int n=0;

    for(auto temp:chs)
    {
        sym[n]=temp.first;
        w[n]=temp.second;
        n++;
    }
    sym[n]='\0';
    int sym_sum=n;
    //创建huffman树
    create_tree(sym,w,sym_sum);
    //进行编码，编码保存在Huffman_code
    get_code(root,0);
    //对每个字符进行编码
    t_str[0]='\0';
    for(int i=0; i<len; i++)
        strcat(t_str,Huffman_code[(int)s_str[i]]);
    //将编码后的字符串写入文件
    ofstream outfile(target_file);
    if(!outfile)
    {
        cout<<"Open File: "<<target_file<<" Error";
        return ;
    }
    int j=0;
    while(t_str[j]!='\0')
    {
        outfile<<t_str[j];
        j++;
    }
    outfile.close();

    delete s_str;//释放数组
    delete t_str;
}*/
/*void HuffmanTree::decoding_to_text(char *source_file,char *target_file)//解压缩
{
    size_t i=0;
    int j=0,len=0;
    TreeNode *cur;
    char ch;//读入字符
    char *s_str =new char[DYNAMIC_MAX_NUM];//读入字符串
    char *t_str = new char[DYNAMIC_MAX_NUM];//解码字符串
    //读取文件
    ifstream infile(source_file);
    if(!infile)
    {
        cout<<"Open File: "<<source_file<<" Error";
        return ;
    }
    while(infile.get(ch))
    {
        s_str[len]=ch;
        len++;
    }
    infile.close();

    s_str[len]='\0';

    //进行解码
    while(i<strlen(s_str))
    {
        cur=root;
        while(1)
        {
            if(s_str[i]=='0'&&cur->lchild)
                cur=cur->lchild;
            else if(s_str[i]=='1'&&cur->rchild)
                cur=cur->rchild;
            else break;
            i++;
        }
        t_str[j]=cur->symbol;
        j++;
    }
    t_str[j]='\0';
    //将解码后的字符串写入文件
    ofstream outfile(target_file);
    if(!outfile)
    {
        cout<<"Open File: "<<target_file<<" Error";
        return ;
    }
    j=0;
    while(t_str[j]!='\0')
    {
        outfile<<t_str[j];
        j++;
    }
    outfile.close();

    delete s_str;//释放数组
    delete t_str;
}*/
