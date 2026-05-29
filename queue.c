#include"queue.h"
void swap( t_entry *arr, int index_A, int index_B)
{
    t_entry temp= arr[index_A];
    arr[index_A]=arr[index_B];
    arr[index_B]=temp;
    
}
/*  */
void sift_up(t_entry *arr,int index)
{
    while(index > 0)
    {
        int parent = (index - 1)/ 2;
        if(arr[index].priority < arr[parent].priority)
        {
            swap( arr, parent, index);
            index =  parent;

        }   
        else
            break;
    }
}

 sift_down(t_entry *arr,int index,int size)
{
    int left;
    int right;
    int smallest;


    while(index < size)
    {
        left  = (index * 2 ) + 1;
        right = (index * 2 ) + 2;

        smallest = index;
        if(left < size && arr[left].priority < arr[smallest].priority)
            smallest =  left;
        
        if( right < size && arr[right].priority < arr[smallest].priority)
            smallest = right;

        if(smallest != index)
        {
            swap(arr, smallest,index);
            index = smallest;
        }
        else
            break;
    }
}
