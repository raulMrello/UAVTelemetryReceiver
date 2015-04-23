/*
 * List.h
 *
 *  Created on: 20/04/2015
 *  Author:     raulMrello
 *
 *  List module, manages double-linked lists of any kind of objects. This library avoids the use of the standard
 *  <list> library in order to reduce its footprint for deeply embedded applications. Even, lists can be limited in
 *  size, allowing only a number of predefined items.
 */

#ifndef __LIST_H
#define __LIST_H

#include <stdint.h>


#ifndef THROW
/** \def THROW
 *  \brief Macro definition to throw Exception code errors by the module
 */
#define THROW(Error)    if(exception) *exception = Error
#endif


/** \class List
 *  \brief List management class accepts items of type T. 
 */
template<typename T>
class List {
public:
    enum Exception {NO_ERRORS = 0,          ///< No exceptions raised
                    NULL_POINTER,           ///< Null pointer in operation
                    LIMIT_EXCEEDED,         ///< Max number of items reached
                    OUT_OF_MEMORY,          ///< No more dynamic memory
                    ITEM_NOT_FOUND};        ///< Item not found in list

    /** \fn List
     *  \brief Default constructor. Creates an empty list
     */
    List();

    /** \fn ~List
     *  \brief Default destructor. Deallocates a list
     */
    ~List();

    /** \fn setLimit
     *  \brief Set a limit of list items
     *  \param numItems Max number of allowed items in list
     *  \param exception Exception error code
     */
    void setLimit(uint32_t numItems, List::Exception *exception = 0);

    /** \fn addItem
     *  \brief Adds an item to the list
     *  \param item Item to add
     *  \param exception Exception error code
     */
    void addItem(T* item, List::Exception *exception = 0);

    /** \fn removeItem
     *  \brief Removes an item
     *  \param item Item to remove
     *  \param exception Exception error code
     */
    void removeItem(T* item, List::Exception *exception = 0);

    /** \fn getItemCount
     *  \brie Gets the list items count
     *  \return Items count
     */
    uint32_t getItemCount();

    /** \fn getFirstItem
     *  \brief Get first item in list
     *  \return Pointer to the first item or NULL if empty
     */
    T* getFirstItem();

    /** \fn getNextItem
     *  \brief Get next item in list
     *  \return Pointer to the next item or NULL if no more
     */
    T* getNextItem();

    /** \fn searchItem
     *  \brief Searches an item into the list
     *  \param item Item to search
     *  \param exception Exception error code
     *  \return True if item found, else false
     */
    bool searchItem(T* item, List::Exception *exception = 0);

private:

    /** List item structure */
    struct ListItem{
        struct ListItem *prev;      ///< pointer to the previous ListItem
        struct ListItem *next;      ///< pointer to the next ListItem
        T * item;                   ///< item
    };

    ListItem *_first;       ///< pointer to the first ListItem
    ListItem *_last;        ///< pointer to the last ListItem
    ListItem *_search;      ///< auxiliary pointer in search operations
    uint32_t  _count;       ///< items count
    uint32_t  _numItems;    ///< max number of allowed items on list


    /** \fn removeItem
     *  \brief Removes an item by its listitem pointer
     *  \param item ListItem to remove
     */
    void removeItem(ListItem * listitem);

};

#include "List_tpp.h"

#endif
