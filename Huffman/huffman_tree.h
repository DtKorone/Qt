#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
struct TreeNode
{
    char symbol;
    int weight;
    TreeNode *lchild;
    TreeNode *rchild;
    TreeNode();
    TreeNode(char s,int w);
};
struct cmp
{
    bool operator()(TreeNode const *a,TreeNode const *b)const
    {
        return a->weight>b->weight;
    }
};
class HuffmanTree
{
public:
    HuffmanTree();
    ~HuffmanTree();
    //void encoding_to_text(char *source_file,char *target_file);//压缩成01字符串 ，已实现二进制压缩和解压，故注释
    //void decoding_to_text(char *source_file,char *target_file);//01字符串解压缩成字符
    bool encoding(const char *source_file,const char *target_file);//二进制压缩
    bool decoding(const char *source_file,const char *target_file);//二进制解压
private:
    TreeNode *root;//哈夫曼树的根节点
    void create_tree(char *sym,int *w,int sym_num);//构造树
    void get_code(TreeNode *node,int len);//遍历节点
    void free_tree(TreeNode *node);//释放树
};
#endif // HUFFMAN_TREE_H
