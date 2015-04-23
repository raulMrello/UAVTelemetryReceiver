/*
 * MsgBroker.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *  MsgBroker library provides an extremely simple mechanism to add publish/subscription semantics to any C/C++ project.
 *  Publish/subscribe mechanisms is formed by:
 *  - Topics: messages exchanged between software agents: publishers and subscribers.
 *  - Publishers: agents which update data topics and notify those topic updates.
 *  - Subscribers: agents which attach to topic updates and act according their new values each time the get updated.
 */

#ifndef MSGBROKER_H_
#define MSGBROKER_H_

#define MSGBROKER_RELEASE   "1.0.20150420001"

#include <string.h>
#include <stdlib.h>
#include "List.h"

/** Add here your porting code for RTOS-based applications */
//#include "your_rtos_mutex_header.h"
#define MUTEX_LOCK()        // your_mutex_lock_function
#define MUTEX_UNLOCK()      // your_mutex_unlock_function


#ifndef THROW
/** \def THROW
 *  \brief Macro definition to throw Exception code errors by the module
 */
#define THROW(Error)    if(exception) *exception = Error
#endif


/** \class MsgBroker
 *  \brief This class provides publish/subscription infrastructure as interprocess communication mechanism.
 *  Topics are the exchanged data structures which by one side will be ready to be subscribed to, or updated
 *  by publish functions.
 *  It is a thread safe implementation, so it can be used in baremetal or RTOS-based applications. If a mutex
 *  is required on RTOS-based applications, def macro MUTEX_LOCK and MUTEX_UNLOCK can be adapted according with
 *  your needs.
 */
class MsgBroker {
public:
    /** \enum Exception
     *  \brief Common error codes thrown as Exceptions by this module
     */
    enum Exception{ NO_ERRORS = 0,          ///< No exceptions raised
                    NULL_POINTER,           ///< Null pointer in operation
                    DEINIT,                 ///< MsgBroker deinitialized
                    OUT_OF_MEMORY,          ///< No more dynamic memory
                    TOPIC_EXISTS,           ///< Topic cannot be created, it already exists
                    TOPIC_NOT_FOUND,        ///< Topic doesn't exists
                    SUBSCRIBER_NOT_FOUND,   ///< Subscriber doesn't exists
                    TOPIC_PENDING,          ///< At least one subscriber has not yet consumed a topic update
                    TOPIC_SIZE};            ///< Topic size is different than the preconfigured one

    /** \def NotifyCallback
     *  \brief Notification callback provided by a subscritor in order to receive topic update notifications
     *  \param subscriber Subscriber object (as void pointer)
     *  \param topicname Name of the topic which has been updated.
      */
    typedef void (*NotifyCallback)(void *subscriber, const char * topicname);

    /** \struct Observer
     *  \brief Observer structure is formed by a handler and a notification callback. Subscriber objects attached
     *  to topic updates, will be registered in the MsgBroker as Observer structures.
     */
    struct Observer{
        void *observer;         ///< notification callback handler object
        NotifyCallback notify;  ///< notification callback function
    };

    /** \struct Topic
     *  \brief Is the topic structure, formed by a list of observers, a topic name, pointer to the latest data update,
     *  data size and count of notification consumption
     */
    struct Topic{
        List<Observer > *observer_list; ///< list of observer objects
        const char  *name;              ///< topic name
        void        *data;              ///< pointer to the latest data update
        uint32_t    datasize;           ///< data size of the data attached to the topic
        uint32_t    count;              ///< number of observers notified and pendind to mark the "consumed" topic signal
    };

    /** \fn init [static]
     *  \brief Initializes the MsgBroker. Creates the topics list if not already created
     */
    static void init() {
        if(!MsgBroker::topic_list){
            MUTEX_LOCK();
            MsgBroker::topic_list = new List<MsgBroker::Topic>();
            MUTEX_UNLOCK();
        }
    }

    /** \fn deinit [static]
     *  \brief Deinitializes the MsgBroker. Frees allocated resources and delete internal objects if not already done
     */
    static void deinit() {
        if(MsgBroker::topic_list){
            MUTEX_LOCK();
            Topic * topic = topic_list->getFirstItem();
            while(topic){
                Observer * obs = topic->observer_list->getFirstItem();
                while(obs){
                    free(obs);
                    obs = topic->observer_list->getNextItem();
                }
                delete(topic->observer_list);
                free(topic);
                topic = topic_list->getNextItem();
            }
            delete(MsgBroker::topic_list);
            MsgBroker::topic_list = 0;
            MUTEX_UNLOCK();
        }
    }

    /** \fn installTopic [static]
     *  \brief Installs a topic in the MsgBroker by a name and its data size
     *  \param name Topic name
     *  \param datasize Size of data managed by this topic
     *  \param exception Int exception raised by the function. This param allow an exception
     *  mechanism where "--exceptions" flags is disabled
     *  \return Pointer to the allocated Topic object
     */
    static MsgBroker::Topic * installTopic(const char* name, uint32_t datasize, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return 0;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(name);
        if(topic){
            THROW(TOPIC_EXISTS);
            MUTEX_UNLOCK();
            return topic;
        }
        topic = (MsgBroker::Topic*)malloc(sizeof(MsgBroker::Topic));
        if(!topic){
            THROW(OUT_OF_MEMORY);
            MUTEX_UNLOCK();
            return 0;
        }
        topic->observer_list = new List<Observer>();
        topic->name = name; topic->datasize = datasize; topic->data = 0; topic->count = 0;
        MsgBroker::topic_list->addItem(topic);
        MUTEX_UNLOCK();
        return topic;
    }

    /** \fn removeTopic [static]
     *  \brief Removes a topic from the MsgBroker by its name
     *  \param name Topic name
     *  \param exception Int exception raised by the function. This param allow an exception
     *  mechanism where "--exceptions" flags is disabled
     */
    static void removeTopic(const char * topic_name, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
            MUTEX_UNLOCK();
            return;
        }
        topic_list->removeItem(topic);
        free(topic);
        MUTEX_UNLOCK();
    }

    /** \fn removeTopic [static]
     *  \brief Removes a topic from the MsgBroker by its pointer
     *  \param t Topic pointer
     *  \param exception Int exception raised by the function. This param allow an exception
     *  mechanism where "--exceptions" flags is disabled
     */
    static void removeTopic(Topic * t, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return;
        }
        MUTEX_LOCK();
        MsgBroker::removeTopic(t->name, exception);
        MUTEX_UNLOCK();
    }

    /** \fn attach [static]
     *  \brief Attaches a subscriber to topic by its topic name
     *  \param name Topic name
     *  \param subscriber Subscriber object
     *  \param notify Notification callback function handled by the subscriber
     *  \param exception Int exception raised by the function. This param allow an exception
     *  mechanism where "--exceptions" flags is disabled
     *  \return Pointer to the allocated Topic object
     */
    static MsgBroker::Observer * attach (const char * topic_name, void *subscriber, NotifyCallback notify, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return 0;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
            MUTEX_UNLOCK();
            return 0;
        }
        MsgBroker::Observer *obs = (MsgBroker::Observer *)malloc(sizeof(MsgBroker::Observer));
        obs->observer = subscriber;
        obs->notify = notify;
        topic->observer_list->addItem(obs);
        MUTEX_UNLOCK();
        return obs;
    }

    /** \fn detach [static]
     *  \brief Detaches a subscriber from a topic by its topic name
     *  \param name Topic name
     *  \param subscriber Subscriber object to be detached
     *  \param exception Exceptions raised by this function
     */
    static void detach (const char * topic_name, void * subscriber, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
            MUTEX_UNLOCK();
            return;
        }
        MsgBroker::Observer *obs = findObserver(topic->observer_list, subscriber);
        if(!obs){
            THROW(SUBSCRIBER_NOT_FOUND);
            MUTEX_UNLOCK();
            return;
        }
        topic->observer_list->removeItem(obs);
        free(obs);
        MUTEX_UNLOCK();
    }

    /** \fn publish [static]
     *  \brief Publishes a topic with a topic name, a data pointer and its size
     *  \param name Topic name
     *  \param data Topic data updated
     *  \param datasize Size of topic to publish
     *  \param exception Exceptions raised by this function
     */
    static void publish (const char * topic_name, void *data, uint32_t datasize, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
            MUTEX_UNLOCK();
            return;
        }
        if(topic->count > 0){
            THROW(TOPIC_PENDING);
            MUTEX_UNLOCK();
            return;
        }
        if(datasize != topic->datasize){
            THROW(TOPIC_SIZE);
            MUTEX_UNLOCK();
            return ;
        }
        topic->data = data;
        topic->datasize = datasize;
        Observer * obs = topic->observer_list->getFirstItem();
        while(obs){
            topic->count++;
            obs->notify(obs->observer, topic->name);
            obs = topic->observer_list->getNextItem();
        }
        MUTEX_UNLOCK();
    }

    /** \fn consumed [static]
     *  \brief Consumes a topic update
     *  \param topic_name Topic name
      *  \param exception Exceptions raised by this function
     */
    static void consumed(const char * topic_name, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            return;
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
        }
        if(topic->count){
            topic->count--;
        }
        MUTEX_UNLOCK();
    }

    /** \fn getTopicData [static]
     *  \brief Get latest topic update
     *  \param topic_name Topic name
     *  \param exception Exceptions raised by this function      
     *  \return Pointer to latest data update
     */
    static void * getTopicData(const char * topic_name, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
            
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
            MUTEX_UNLOCK();
            return 0;
        }
        MUTEX_UNLOCK();
        return topic->data;
    }
 
    /** \fn getTopicDatasize [static]
     *  \brief Get latest topic update datasize
     *  \param topic_name Topic name
     *  \param exception Exceptions raised by this function      
     *  \return Number of topic datasize
     */
   static uint32_t getTopicDatasize(const char * topic_name, MsgBroker::Exception *exception = 0){
        THROW(NO_ERRORS);
        if(!MsgBroker::topic_list){
            THROW(DEINIT);
        }
        MUTEX_LOCK();
        MsgBroker::Topic * topic = findTopic(topic_name);
        if(!topic){
            THROW(TOPIC_NOT_FOUND);
        }
        MUTEX_UNLOCK();
        return topic->datasize;
    }

private:
    /** Static topic list pointer */
    static List<MsgBroker::Topic> * topic_list;
 
    /** \fn findTopic [static]
     *  \brief Finds a topic in the topic list from a topic name
     *  \param topic_name Topic name
     *  \return Pointer to the topic or NULL if not found
     */
    static MsgBroker::Topic * findTopic(const char * topic_name){
        MsgBroker::Topic *topic = topic_list->getFirstItem();
        while(topic){
            if(strcmp(topic_name, topic->name) == 0){
                return topic;
            }
            topic = topic_list->getNextItem();
        }
        return 0;
    }

    /** \fn findObserver [static]
     *  \brief Finds an observer in a topic from a subscriber pointer
     *  \param list Observer list of a topic
     *  \param subscriber Subscriber pointer attached to the observer to find
     *  \return Pointer to the observer or NULL if not found
     */
    static MsgBroker::Observer * findObserver(List<MsgBroker::Observer> * list, void *subscriber){
        MsgBroker::Observer *obs = list->getFirstItem();
        while(obs){
            if(subscriber == obs->observer){
                return obs;
            }
            obs = list->getNextItem();
        }
        return 0;
    }


};

#endif /* MSGBROKER_H_ */
