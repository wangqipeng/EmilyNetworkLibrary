#include "PriorityQueue.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    PriorityQueue<int *> q;
    int i20 = 20;
    int i14 = 14;
    int i27 = 27;
    int i1  =  1;
    int i13 = 13;
    int i7  =  7;
    int i25 = 25; 

    int *p20 = &i20;
    int *p14 = &i14;
    int *p27 = &i27;
    int *p1  = &i1;
    int *p13 = &i13;
    int *p7  = &i7;
    int *p25 = &i25;
    
    q.Push(p20);
    q.Push(p14);
    q.Push(p27);
    q.Push(p1);
    q.Push(p13);
    q.Push(p7);
    q.Push(p25);
    cout<<__LINE__<<"\n"<<endl;
    
    q.PrintQueue();
    q.Pop();
    q.PrintQueue();
    q.Pop();
    q.PrintQueue();
	q.Pop();
	q.PrintQueue();
	q.Pop();
	q.PrintQueue();
	q.Pop();
	q.PrintQueue();
	q.Pop();
	q.PrintQueue();
	q.Pop();
	q.PrintQueue();
}
