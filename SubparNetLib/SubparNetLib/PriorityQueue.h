#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include "Log.h"
/*a heap, the min element in the top
 *
 *
 leftchild(i)  = 2 * i
 rightchild(i) = 2 * i + 1
 parent(i)     = i/2;
*/
namespace BaseLib
{
template <typename T>
class PriorityQueue
{
public:
    typedef typename std::vector<T>::iterator Iterator;
    
    PriorityQueue():array_(1){}

    ~PriorityQueue(){}
    
    //pop the top element and delete the top
    //then, the last element is placed in the top position
    //at last, adjust the array to a heap
    T& Pop()
    {
        if (ElemNumbers() < 1)
	    {
	        FILE_LOG(logERROR)<<" Priority::Pop error: empty queue";
	    }
        
	    Iterator ibeg = array_.begin();
	    Iterator ibeg_next = ++ibeg;
	    Iterator iend = array_.end();
	    
	    size_t index = 1;
	    const T result = *ibeg_next;
	    *ibeg = *iend;
	    if (index * 2 > ElemNumbers())
	    {
            array_.push_back(0);
            return result;    
	    }

	    size_t lchild = index * 2;
	    size_t rchild = lchild + 1;
	    //adjust
        while (rchild <= ElemNumbers() &&
		       array_[lchild] < array_[index] && 
	           array_[rchild] < array_[index])
	    {
	        if(array_[rchild] < array_[lchild])
	        {
		        //FIXME:use stl swap
	            Swap(&array_[index], &array_[rchild]);
		        index = rchild;
	        } 
	        else
	        {
	            Swap(&array_[index], &array_[lchild]);
		        index = lchild;
	        }
            lchild = index * 2;
            rchild = lchild + 1;
	    }
	    assert(ElemNumbers() > 0);
		array_.pop_back();
	    return result;
    }
    
    //insert a element from the tail of the array
    void Push(const T& elem)
    {
        int index = ElemNumbers() + 1;
	    if(0 == ElemNumbers())
	    {
	        array_[index] = elem;
	        return;
	    }
		
        array_[index] = elem;
	    int parent = index / 2;
	    //adjust
	    while (elem < array_[parent] && index > 1)
        {
	        Swap(&array_[parent], &array_[index]);
	        index = parent;
	    }
	    return;
    }

    T Top()
    {
	    assert(ElemNumbers() > 0);
        return array_.at(1);
    }

    bool IsFull()
    {
        return (array_.size() < array_.capacity())? false : true; 
    }

    size_t Size()
    {
        return ElemNumbers();
    }

    void PrintQueue()
    {
	    int i = 1;
	    for(;i <= ElemNumbers(); ++i)
     	{
	        FILE_LOG(logINFO)<<"array["<<i<<"]"<<" is "<<array_[i];
	    }
    }
private:
    size_t ElemNumbers()
    {
        return array_.size() - 1;
    }

    void Swap(T* p, T* c)
    {
	    T t = *p;
        *p  = *c;
	    *c  = t;
    }
	
private:
    std::vector<T> array_;
};

//Specialized for raw pointer
template <typename T>
class PriorityQueue<T*>
{
public:
    typedef typename std::vector<T*>::iterator Iterator;
    PriorityQueue():array_(1)
    {
    }

    ~PriorityQueue(){}
    
    //pop the top element and delete the top
    //then, the last element is placed in the top position
    //at last, adjust the array to a heap
    T* Pop()
    {
        if (ElemNumbers() < 1)
	    {
	        FILE_LOG(logERROR)<<" Priority::Pop error: empty queue";
	    }

        Iterator ibeg       = array_.begin();
	    Iterator first_elem = ++ibeg;
	    Iterator iend       = array_.end();
	    Iterator last_elem  = --iend; 
 
	    size_t index = 1;
	    T* result = *first_elem;
        *first_elem = *last_elem;
	    if (index * 2 > ElemNumbers())
	    {
            array_.pop_back();
            return result;    
	    }

	    size_t lchild = index * 2;
	    size_t rchild = lchild + 1;
	    //adjust
        while (rchild <= ElemNumbers() &&
			   *array_[lchild] < *array_[index] && 
               *array_[rchild] < *array_[index])
	    {
	        if(*array_[rchild] < *array_[lchild])
	        {
	            Swap(array_[index], array_[rchild]);
		        index = rchild;
	        } 
	        else
	        {
	            Swap(array_[index], array_[lchild]);
		        index = lchild;
	        }
            lchild = index * 2;
            rchild = lchild + 1;
	    }
        assert(ElemNumbers() > 0);
        array_.pop_back();
	    return result;
    }
    
    //insert a element from the tail of the array
    void Push(T* elem)
    {
	    if(0 == ElemNumbers())
	    {
            array_.push_back(elem);
	        return;
	    }
	
	    int index = ElemNumbers() + 1;
        array_.push_back(elem);
	    int parent;
	    //adjust
	    while (index > 1 && *elem < *array_[parent = index/2])
      	{
	        Swap(array_[parent], array_[index]);
	        index = parent;
	    }
	    return;
    }

    T* Top()
    {
	    assert(ElemNumbers() > 0);
        return array_.at(1);
    }

    bool IsFull()
    {
        return (array_.size() < array_.capacity())? false : true; 
    }

    size_t Size()
    {
        return ElemNumbers();
    }
    
    void PrintQueue()
    {
	    assert(ElemNumbers() > 0);
	    size_t i = 1;
        
	    for(;i <= ElemNumbers(); ++i)
     	{
	        FILE_LOG(logINFO)<<"array["<<i<<"]"<<" is "<<(*array_[i]);
	    }
    }  
private:
    size_t ElemNumbers()
    {
        return array_.size() - 1;
    }

    inline void Swap(T*& p, T*& c)
    {
	    T* t = p;
        p = c;
	    c = t;
    }
	
private:
    std::vector<T*> array_;
};
}
#endif
