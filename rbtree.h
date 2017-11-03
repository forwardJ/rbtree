#ifndef RBTREE_H
#define RBTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <functional>
#include <string>
using namespace std;

template<class T>
class rbNode{
public:
    rbNode* p;
    rbNode* l;
    rbNode* r;
    bool red; // true --> red, false--> black
    T value;
};

template <class T>
class rbTree
{
public:
    rbNode<T>* head = NULL;
    std::function<int(T, T)> compare;

public:
    rbTree(std::function<int(T, T)> compare = nullptr)
    {
        head = NULL;
        if(compare != NULL)
            this->compare = compare;
        else
            this->compare = [](T t1, T t2){return t1-t2;};
    }

    // 重要接口，插入对象
    bool insert(T value)
    {
        // 先使用value构造节点对象
        rbNode<T>* node = __node(value);

        // 查找树的插入
        bool result = __insert(node);
        if(result == false)
            return false;

        // 调整
        __adjust_insert(node);

        return true;
    }

    rbNode<T>* find(T value)
    {
        rbNode<T>* n = head;
        while(n)
        {
            int result = compare(value, n->value);
            if(result == 0)
                return n;

            if(result  > 0)
            {
                n = n->r;
            }
            else
            {
                n=n->l;
            }
        }
        return NULL;
    }

    void remove(rbNode<T>* n)
    {
        if(n->l == NULL && n->r == NULL)
        {
            if(n==head)
            {
                delete head;
                head = NULL;
                return;
            }

            doRemove(n);

            // 重新更新head节点
            while(head->p)
            {
                head = head->p;
            }
            head->red = false;
            return;
        }

        rbNode<T>* rn = NULL;
        if(n->l)
        {
            // 寻找左子树，最右边的节点
           rn = n->l;
            while (rn->r) {
                rn = rn->r;
            }
        }
        else if(n->r)
        {
            // 寻找右子树，最左边的节点
            rn = n->r;
            while (rn->l) {
                rn = rn->l;
            }
        }

        // 把rn的value，赋值给n，然后再删除rn
        printf("n=%p, rn=%p\n", n, rn);
        n->value = rn->value;
        remove(rn);
    }

    bool remove(T value)
    {
        rbNode<T>* n = find(value);
        if(n == NULL) return false;

        remove(n);
        return true;
    }

    void dump( const char* filename1,
               std::function<string(T)> getname = nullptr)
    {
        if(getname == nullptr)
        {
            getname = [](T value){
                char buf[1024];
                sprintf(buf, "%d", value);
                return string(buf);
            };
        }

        string dot_filename(filename1);
        dot_filename = dot_filename + ".dot";
        string jpg_filename(filename1);
        jpg_filename = jpg_filename + ".jpg";

        FILE* fstream = fopen(dot_filename.c_str(), "w");
        char out_cmd[256];
        int ret;
        if(fstream == NULL)
        {
            return;
        }

        fprintf(fstream, "digraph g{\n");
        fprintf(fstream, "node [shape = record, height=.1];\n");
        rb_add_node(fstream, head, getname);
        rb_add_line(fstream, head, getname);
        fprintf(fstream, "}\n");
        fclose(fstream);
        sprintf(out_cmd, "dot -T\"jpg\" -o %s %s", jpg_filename.c_str(), dot_filename.c_str());
        ret = system(out_cmd);
        //       if(ret == 0)
        //           unlink(filename);
    }

private:
    void unlink_node(rbNode<T>* n)
    {
        rbNode<T>* p = n->p;
        if(p->l == n) p->l = NULL;
        else p->r = NULL;
        n->p = NULL;
        delete n;
        return;
    }

    void doRemove(rbNode<T>* n, bool needUnlink = true)
    {
        rbNode<T>* p = n->p;
        if(n->red)
        {
            if(needUnlink)
                unlink_node(n);
            return;
        }

        // 如果父节点是红色的
        if(p->red)
        {
            if(n == p->l) // 在左边
            {
                rbNode<T>* b = p->r;
                rbNode<T>* nl = b->l;
                rbNode<T>* nr = b->r;
                if(nl == NULL)
                {
                    rotate_left(p);
                }
                else // 左侄子存在，肯定是红的
                {
                    if(nr) // 右侄子存在，它肯定也是红的
                    {
                        rotate_left(p);
                        b->red = true;
                        p->red = false;
                        nr->red = false;

                    }
                    else // 右侄子不存在，并且左侄子存在
                    {
                        // 以兄弟节点进行
                        rotate_right(b);
                        b->red = true;
                        nl->red = false;
                        rotate_left(p);
                    }
                }
            }
            else
            {
                rbNode<T>* b = p->l;
                rbNode<T>* nl = b->l;
                rbNode<T>* nr = b->r;
                if(nr == NULL)
                {
                    rotate_right(p);
                }
                else // 右侄子不为空
                {
                    if(nl)
                    {
                        rotate_right(p);
                        b->red = true;
                        p->red = false;
                        nl->red = false;
                    }
                    else
                    {
                        // 以兄弟节点进行
                        rotate_left(b);
                        b->red = true;
                        nr->red = false;
                        rotate_right(p);
                    }
                }
            }
        }
        else // 父亲是黑色的
        {
            if(n == p->l) // 左边
            {
                rbNode<T>* b = p->r;
                rbNode<T>* nl = b->l;
                rbNode<T>* nr = b->r;
                if(b->red) // 兄弟是红色，侄子们都存在，并且是黑色
                {
                    rotate_left(p);
                    b->red = false;
                    nl->red = true;
                }
                else // 兄弟是黑色的
                {


                        if(nr)
                        {
                            // 如果有右侄子，那么就以父左旋
                            // 把右侄子变成黑色即可
                            rotate_left(p);
                            nr->red = false;
                        }
                        else if(nl)
                        {
                            rotate_right(b);
                            rotate_left(p);
                            nl->red = false;
                        }
                        else // 绝后
                        {
                            b->red = true;// 保持局部平衡
                            // 继续对父亲节点删除调整
                            doRemove(p, false);
                        }


                        // 如果有左侄，那么对兄弟进行右旋，再以父节点进行左旋
                        // 把左侄子变成黑色

                        // 没有侄子，局部没有办法再平衡了
                        // 只能兄弟变成红色，先局部平衡再说
                        // 为了保持全局平衡，把父节点作为删除节点再调整
                    }

            }
            else // 删除的节点在右边
            {
                rbNode<T>* b = p->l;
                rbNode<T>* nl = b->l;
                rbNode<T>* nr = b->r;
                if(b->red) // 兄弟是红色，侄子们都存在，并且是黑色
                {
                    rotate_right(p);
                    b->red = false;
                    nl->red = true;
                }
                else // 兄弟是黑色的
                {

                        if(nl)
                        {

                            rotate_right(p);
                            nl->red = false;
                        }
                        else if(nr)
                        {
                            rotate_left(b);
                            rotate_right(p);
                            nr->red = false;
                        }
                        else // 绝后
                        {
                            b->red = true;// 保持局部平衡
                            // 继续对父亲节点删除调整
                            doRemove(p, false);
                        }
                        // 如果有左侄，那么对兄弟进行右旋，再以父节点进行左旋
                        // 把左侄子变成黑色

                        // 没有侄子，局部没有办法再平衡了
                        // 只能兄弟变成红色，先局部平衡再说
                        // 为了保持全局平衡，把父节点作为删除节点再调整
                    }

            }
        }
        if(needUnlink)
            unlink_node(n);
    }

    static void rb_add_node(FILE* fstream,
                            rbNode<T>* node,
                            std::function<string(T)>  func)
    {
        if(node == NULL)
            return;

        string debug = func(node->value);
        fprintf(fstream, "node_%s[label=\"<f0> |<f1> %s|<f2> \", %s];\n",
                debug.c_str(),
                debug.c_str(), node->red? "fontcolor=red":"fontcolor=black");

        rb_add_node(fstream, node->l, func);
        rb_add_node(fstream, node->r, func);
    }

    static void rb_add_line(FILE* fstream,
                            rbNode<T>* node,
                            std::function<string(T)>  func)
    {
        if(node == NULL)
            return;

        string debug = func(node->value);
        string child;

        if(node->l)
        {
            child = func(node->l->value);
            fprintf(fstream, "\"node_%s\":f0 -> \"node_%s\":f1;\n", debug.c_str(),
                    child.c_str());

        }
        if(node->r)
        {
            child = func(node->r->value);
            fprintf(fstream, "\"node_%s\":f2 -> \"node_%s\":f1;\n", debug.c_str(),
                    child.c_str());
        }

        rb_add_line(fstream, node->l, func);
        rb_add_line(fstream, node->r, func);
    }

    void __adjust_insert(rbNode<T>* node)
    {
        // 原来是一颗空树
        if(node->p == NULL)
        {
            head = node;
            node->red = false;
            return;
        }

        // 如果父节点就是黑的
        if(node->p && node->p->red == false)
        {
            return;
        }

        // 如果父节点是红色的
        // 1. 新节点、父节点、爷爷节点在一条线上
        // 1. 新节点、父节点、爷爷节点不在一条线上
        rbNode<T>* g = node->p->p;  // g->red == false
        rbNode<T>* p = node->p;       // p->red = true
        rbNode<T>* n = node;             // n->red = true
        // 1. 如果父亲是祖父的左儿子，而且新节点是父亲左儿子
        if(g->l == p && p->l == n)
        {
            rotate_right(g);
            n->red = false;
            __adjust_insert(p); // 递归进行调整
            return;
        }
        // 2. 如果父亲是祖父的右儿子，而且新节点是父亲右儿子
        if(g->r == p && p->r ==n)
        {
            rotate_left(g);
            n->red = false;
            __adjust_insert(p); // 递归进行调整
            return;
        }

        // 3. 如果父亲是左儿子，并且新节点是右儿子
        if(g->l == p && p->r == n)
        {
            rotate_left(p);
            __adjust_insert(p);
            return;
        }

        if(g->r == p && p->l == n)
        {
            rotate_right(p);
            __adjust_insert(p);
            return;
        }
    }
    // 只是负责作为搜索树的插入问题，不管红黑树的属性
    bool __insert(rbNode<T>* node)
    {
        // 如果是一颗空树，那么直接...
        if(head == nullptr)
        {
            return true;
        }

        rbNode<T>* p = head;
        while(p)
        {
            if(compare(node->value, p->value) > 0) // node  > p
            {
                if(p->r)
                {
                    p = p->r;
                    continue;
                }
                else
                {
                    p->r = node;
                    node->p = p;
                    return true;
                }
            }
            else if(compare(node->value, p->value) < 0)
            {
                if(p->l)
                {
                    p = p->l;
                    continue;
                }
                else
                {
                    p->l = node;
                    node->p = p;
                    return true;
                }
            }
            else
            {
                return false;
            }
        }

        // 运行不到的代码
        return false;
    }

    rbNode<T>* __node(T value)
    {
        rbNode<T>* node = new rbNode<T>();
        node->l = NULL;
        node->r = NULL;
        node->p = NULL;
        node->red = true;
        node->value = value;
        return node;
    }

    void rotate_right(rbNode<T>* g)
    {
        rbNode<T>* pg = g->p; // pg maybe NULL
        rbNode<T>* p = g->l;
        rbNode<T>* pr = p->r; // pr maybe NULL
        //    rbNode<T>* n = p->l;  // n maybe NULL

        g->l = pr;
        if(pr) pr->p = g;

        p->r = g;
        g->p = p;

        if(pg)
        {
            if(pg->l == g) pg->l = p;
            else if(pg->r == g) pg->r = p;
        }
        p->p = pg;
    }

    void rotate_left(rbNode<T>* g)
    {
        rbNode<T>* pg = g->p; // pg maybe NULL
        rbNode<T>* p = g->r;
        rbNode<T>* pl = p->l; // pr maybe NULL
        //    rbNode<T>* n = p->l;  // n maybe NULL

        g->r = pl;
        if(pl) pl->p = g;

        p->l = g;
        g->p = p;

        if(pg)
        {
            if(pg->l == g) pg->l = p;
            else if(pg->r == g) pg->r = p;
        }
        p->p = pg;
    }
};

#endif // RBTREE_H
