#include "function.h"
#include "huffman_tree.h"
#include<QTextCodec>
Function::Function()
{

}
bool Function::operatefile(QString source_file,QString target_file,int mode)
{
    QTextCodec *code = QTextCodec::codecForName("GB2312");
    const char * file1 = code->fromUnicode(source_file).data();
    const char * file2 = code->fromUnicode(target_file).data();
    bool flag=false;
    HuffmanTree T;
    if(mode==0)
        flag=T.encoding(file1,file2);
    else if(mode==1)
        flag=T.decoding(file1,file2);
    return flag;
}
