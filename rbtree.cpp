
#include "rbtree.h"
#include <cmath>
#include <stdlib.h>
#include <time.h>
int main()
{
    rbTree<int> tree;

    //srand(time(NULL));
    srand(7);

    char buf[1024];
    int count = 20;
    for(int i=0; i<count; ++i)
    {
        int v = random() % 1000;
        if(!tree.insert(v))
        {
            printf("repeat ...\n");
            continue;
        }
        sprintf(buf, "%03d", i);
     //   tree.dump(buf);
    }

    tree.dump("normal");
 //   tree.remove(537);
  //  tree.dump("result");

}
