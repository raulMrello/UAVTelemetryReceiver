/*
 * List.tpp
 *
 *  Created on: 20/04/2015
 *  Author:     raulMrello
 *
 *  List module, manages double-linked lists.
 */

#include <stdlib.h>

template<typename T>
void List<T>::removeItem(ListItem *listitem){
    // if is first will point to the next
    if(!listitem->prev){
        // if next is null then list is empty
        if(!listitem->next){
            _first = 0;
            _last = 0;
            _count = 0;
            _search = 0;
            free(listitem);
            return;
        }
        _first = listitem->next;
        listitem->next->prev = 0;
        _count--;
         free(listitem);
         return;
    }
    // else, adjust pointers
    listitem->prev->next = listitem->next;
    if(listitem->next)
        listitem->next->prev = listitem->prev;
    else
        _last = listitem->prev;
    _count--;
    free(listitem); 
}

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

template<typename T>
List<T>::List(){
    _count = 0;
    _first = 0;
    _last = 0;
    _search = 0;
    _numItems = (uint32_t)-1;
}

//------------------------------------------------------------------------------------
template<typename T>
List<T>::~List(){
    // remove all items
    while(_first){
        removeItem(_last);
    }
}

//------------------------------------------------------------------------------------
template<typename T>
void List<T>::setLimit(uint32_t numItems, List::Exception *exception){
    THROW(NO_ERROS);
    if(!numItems){
        THROW(NULL_POINTER);
    }
    _numItems = numItems;
}

//------------------------------------------------------------------------------------
template<typename T>
void List<T>::addItem(T* item, List::Exception *exception){
    THROW(NO_ERRORS);
    if(!item){
        THROW(NULL_POINTER);
    }
    if(_count >= _numItems){
        THROW(LIMIT_EXCEEDED);
    }
    ListItem* listitem;
    listitem = (ListItem*)malloc(sizeof(ListItem));
    if(!listitem){
        THROW(OUT_OF_MEMORY);
    }
    listitem->item = item;
    // if first element, initializes pointers
    if(!_count){
        _first = listitem;
        _last = listitem;
        listitem->prev = 0;
        listitem->next = 0;
    }
    // else at to the end
    else{
        ListItem* last = _last;
        last->next = listitem;
        listitem->prev = last;
        listitem->next = 0;
        _last = listitem;
    }
    _count++;
}

//------------------------------------------------------------------------------------
template<typename T>
void List<T>::removeItem(T* item, List::Exception *exception){
    THROW(NO_ERRORS);
    if(!item){
        THROW(NULL_POINTER);
    }
    ListItem* listitem;
    // search the item in all elements
    listitem = _first;
    do{
        if(listitem->item == item){
            removeItem(listitem);
            return;
        }
        listitem = listitem->next;
    }while(listitem);
    THROW(ITEM_NOT_FOUND);
}

//------------------------------------------------------------------------------------
template<typename T>
uint32_t List<T>::getItemCount(){
    return _count;
}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getFirstItem(){
    _search = _first;
    if(!_search){
        return (T*)0;
    }
    return _search->item;

}

//------------------------------------------------------------------------------------
template<typename T>
T* List<T>::getNextItem(){
    if(!_search){
        return (T*)0;
    }
    _search = _search->next;
    if(!_search){
        return (T*)0;
    }
    return _search->item;
}

//------------------------------------------------------------------------------------
template<typename T>
bool List<T>::searchItem(T* item, List::Exception *exception){
    THROW(NO_ERRORS);
    if(!item){
        THROW(NULL_POINTER);
    }
    T* i = getFirstItem();
    if(!i){
        return 0;
    }
    for(;;){
        if(i == item){
            return i;
        }
        i = getNextItem();
        if(!i){
            break;
        }
    }
    return 0;
}
