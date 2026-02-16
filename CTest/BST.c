#include <stdio.h>
typedef struct tree_node
{
    int data;
    struct node *left;
    struct node *right;
} TreeNode;

typedef struct{
    TreeNode *root;
}BST;

//1、创建二插搜索树
BST *createBST()
{
    BST *bst = (BST *)malloc(sizeof(BST));
    bst->root = NULL;
    return bst;
}

//2、创建新节点
static TreeNode *createNode(int data)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

//3、销毁二叉搜索树(后序递归释放)
static void destroy_tree(TreeNode *root)
{
    if (root == NULL)
    {
        return;
    }
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
}

void bst_destroy(BST *bst)
{
    if (bst) {
        destroy_tree(bst->root);
        bst->root = NULL;
        free(bst);
    }
}

//4、插入节点(迭代写法)
void bst_insert(BST *bst, int data)
{
    if(bst == NULL) return; 
    //1、创建新节点
    TreeNode *node = createNode(data);
    //2、寻找插入位置
    if(bst->root == NULL){
        bst->root = node;
        return;
    }

    TreeNode *cur = bst->root;
    TreeNode *parent = NULL;

    while(cur != NULL)
    {
        parent = cur;
        if (cur->data > data)
        {
            cur = cur->left;
        }else if(cur->data < data){
            cur = cur->right;
        }else{
            //键已存在
            free(node);
            return;
        }
    }
    
    if(data < parent->data)
    {
        parent->left = node;
    }
    else{
        parent->right = node;
    }

    return;
}

//5、查找节点
TreeNode *bst_search(BST *bst, int data)
{
    if (bst == NULL)
    {
        return NULL;
    }
    TreeNode *cur = bst->root;
    while(cur != NULL)
    {
        if (cur->data > data)
        {
            cur = cur->left;
        }else if(cur->data < data){
            cur = cur->right;
        }else{
            return cur;
        }
    }

    return NULL;
}

// 6. 查找最小节点（辅助删除用）
static TreeNode* find_min(TreeNode* node) {
    while (node && node->left != NULL) {
        node = node->left;
    }
    return node;
}

//7、删除节点
void bst_delete(BST *tree, int data)
{
    if(tree == NULL || tree->root == NULL) return; 

    TreeNode *cur = tree->root;
    TreeNode *parent = NULL;

    while(cur != NULL && cur->data != data)
    {
        parent = cur;
        if (cur->data > data)
        {
            cur = cur->left;
        }else{
            cur = cur->right;
        }
    }
    if (cur == NULL)
    {
        return;
    }

    //情况1 删除的节点是叶子节点
    if (cur->left == NULL && cur->right == NULL)
    {
        if(parent == NULL)
        {
            tree->root = NULL;
        }else if(parent->left == cur){
            parent->left = NULL;
        }else{
            parent->right = NULL;
        }
        free(cur); 
    }
    //情况2 删除的节点只有一个子节点
    else if(cur->left == NULL || cur->right == NULL)
    { 
        TreeNode *child = cur->left ? cur->left : cur->right;
        if(parent == NULL)
        {
            tree->root = child;
        }else if(parent->left == cur){
            parent->left = child;
        }else{
            parent->right = child;
        }
        free(cur);
    }
    //情况3 删除的节点有两个子节点
    else
    {
        TreeNode *successor = find_min(cur->right);
        TreeNode *successor_parent = cur->right;
        
        //找到successor的父节点
        while(successor_parent->left != successor)
        {
            successor_parent = successor_parent->left;
        }

        cur->data = successor->data;
        successor_parent->left = successor->right;
        free(successor);
    }
}

// 8. 前序遍历
void bst_preorder(BST* tree) {
    if (tree == NULL || tree->root == NULL) return;
    printf("前序遍历: ");
    void preorder(TreeNode* node) {
        if (node == NULL) return;
        printf("%d ", node->key);
        preorder(node->left);
        preorder(node->right);
    }
    preorder(tree->root);
    printf("\n");
}

// 9. 中序遍历（BST 中序遍历得到有序序列）
void bst_inorder(BST* tree) {
    if (tree == NULL || tree->root == NULL) return;
    printf("中序遍历: ");
    void inorder(TreeNode* node) {
        if (node == NULL) return;
        inorder(node->left);
        printf("%d ", node->key);
        inorder(node->right);
    }
    inorder(tree->root);
    printf("\n");
}

// 10. 后序遍历
void bst_postorder(BST* tree) {
    if (tree == NULL || tree->root == NULL) return;
    printf("后序遍历: ");
    void postorder(TreeNode* node) {
        if (node == NULL) return;
        postorder(node->left);
        postorder(node->right);
        printf("%d ", node->key);
    }
    postorder(tree->root);
    printf("\n");
}

// 11. 层序遍历（广度优先，使用队列）
void bst_levelorder(BST* tree) {
    if (tree == NULL || tree->root == NULL) return;

    printf("层序遍历: ");

    Queue* q = queue_create();  // 需要前面实现的队列
    if (!q) return;

    queue_enqueue(q, (intptr_t)tree->root);  // 存指针，需强转

    while (!queue_is_empty(q)) {
        intptr_t ptr_val;
        queue_dequeue(q, &ptr_val);
        TreeNode* node = (TreeNode*)ptr_val;

        printf("%d ", node->key);

        if (node->left) queue_enqueue(q, (intptr_t)node->left);
        if (node->right) queue_enqueue(q, (intptr_t)node->right);
    }

    queue_destroy(q);
    printf("\n");
}