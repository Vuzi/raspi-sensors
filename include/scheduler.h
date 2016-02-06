/**
 * @file scheduler.h
 * @brief Class for the sensors scheduler. This file provides a simplified interface to
 * perform asynchronous actions
 * @author Vuzi
 * @version 0.2
 */

#ifndef H_SCHEDULER
#define H_SCHEDULER
 
#include <unistd.h>
#include <node.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <uv.h>

/**
 * @namespace scheduler
 *
 * Name space used to store every class and functions related to the scheduler
 */
namespace scheduler {

    /**
     * @brief Prototype for the asynchonous action
     *
     * @param req The libuv information
     */
    void AsyncAction(uv_work_t* req);

    /**
     * @brief Prototype for the synchonous action taking place after the
     * asynchonous action previously defined
     *
     * @param req    The libuv information
     * @param status The libuv status
     */
    void AsyncActionAfter(uv_work_t* req, int status);

    template<typename T1, typename T2>
    class scheduler;
    
    /**
     * Structure used with libuv as the data
     */
    template<typename T1, typename T2>
    struct schedulerBaton {
        uv_work_t request;         // libuv
        scheduler<T1, T2> *handle; // scheduler itself
        T2 resultValue;            // result of the async action
    };

    /**
     * @brief Scheduler used to schedule action in time. 
     * 
     * Scheduler used to schedule action in time. The action may be repeatable. libuv is used
     * with its main thread pool.
     * 
     * Note that this class should only be used with the new operator, and will delete itself when not
     * needed anymore (when the action is performed, or when cancel is called if repeatable)
     * 
     * @tparam T1 The type that will performs the asynchronous task
     * @tparam T2 The type of the result of the asynchronous task
     */
    template<typename T1, typename T2>
    class scheduler {

        using schedulerAction = std::function<T2(T1)>;
        using schedulerCallback = std::function<void(T1, T2)>;
        using schedulerCleanup = std::function<void()>;
        
        friend void AsyncAction(uv_work_t* req) {
            schedulerBaton<T1, T2>* baton = static_cast<schedulerBaton<T1, T2>*>(req->data);
            scheduler<T1, T2>* handle = baton->handle;

            // Lock and wait for the time/cancellation
            if(handle->frequence > 0) {
                std::unique_lock<std::mutex> l(handle->m);
                handle->stop_threads.wait_for(l, std::chrono::seconds(handle->frequence));
            }

            // Call the async action
            baton->resultValue = handle->action(handle->producer);
        }

        friend void AsyncActionAfter(uv_work_t* req, int status) {
            schedulerBaton<T1, T2>* baton = static_cast<schedulerBaton<T1, T2>*>(req->data);
            scheduler<T1, T2>* handle = baton->handle;
            
            // If canceled, quit now
            if(handle->cancelled)
                goto quit;

            // Call callback with result
            handle->callback(handle->producer, baton->resultValue);

            // Re-launch the thread
            if(handle->repeat)
                uv_queue_work(uv_default_loop(), &baton->request, AsyncAction, AsyncActionAfter);
            else
                goto quit; // Not to relaunch, quit

            return;

            quit:
            delete baton;
            if(handle->cleanup)
                handle->cleanup(); // If any cleanup method is defined, call it
            delete handle;
        }

        public:
            /**
             * @brief Scheduler construtor.
             *
             * @param _producer  Producer provided to the callback. Should be copied or keep allocated as long as the 
             *                   scheduler is working
             * @param _action    The asynchronous action, a lambda should be provided. The previously specified value
             *                   will be provided, and the result is expected. This lambda should be thread safe
             * @param _callback  The synchronous action executed after the asynchronous action. Both the producer
             *                   and the result will be provided
             * @param _frequence The repeat frequence, if should be repeated
             * @param _repeat    True if the scheduler should repeat the action, or false to only perform once
             */
            scheduler(T1 _producer, schedulerAction _action, schedulerCallback _callback, unsigned _frequence = 0, bool _repeat = false)
                      : scheduler(_producer, _action, _callback, schedulerCleanup(), _frequence, _repeat) {}
            
            /**
             * @brief Scheduler construtor.
             *
             * @param _producer  Producer provided to the callback. Should be copied or keep allocated as long as the 
             *                   scheduler is working
             * @param _action    The asynchronous action, a lambda should be provided. The previously specified value
             *                   will be provided, and the result is expected. This lambda should be thread safe
             * @param _callback  The synchronous action executed after the asynchronous action. Both the producer
             *                   and the result will be provided
             * @param _cleanup   Lambda called right before the scheduler free itself 
             * @param _frequence The repeat frequence, if should be repeated
             * @param _repeat    True if the scheduler should repeat the action, or false to only perform once
             */
            scheduler(T1 _producer, schedulerAction _action, schedulerCallback _callback, schedulerCleanup _cleanup, unsigned _frequence = 0, bool _repeat = false) {
                action = _action;
                callback = _callback;
                cleanup = _cleanup;
                frequence = _frequence;
                producer = _producer;
                repeat = _repeat;
                
                launched = false; // Not launched 
            }

            /**
             * @brief Destructor
             */
            ~scheduler() {}
            
            /**
             * Launch all the sensors' workers. If already launched, this method has no effect
             */
            void launch() {
                if(launched)
                    return;

                // Not canceled
                cancelled = false;
                launched = true;

                // Create and set the baton
                schedulerBaton<T1, T2> *baton = new schedulerBaton<T1, T2>;
                
                baton->request.data = baton;
                baton->handle = this;
                
                uv_queue_work(uv_default_loop(), &baton->request, AsyncAction, AsyncActionAfter);
            }

            /**
             * Cancel the workers. If already canceled or never launched, this method has no effect
             */
            void cancel() {
                if(!launched)
                    return;
                
                cancelled = true;
                stop_threads.notify_all();
            }

        private:
            schedulerAction action;               // Action
            schedulerCallback callback;           // Callback
            schedulerCleanup cleanup;             // Cleanup
        
            unsigned frequence;
            T1 producer;

            bool cancelled;                       // If threads should stop
            bool launched;                        // If already launched
            bool repeat;                          // If should be repeted

            std::condition_variable stop_threads; // Stop condition for all threads
            std::mutex m;                         // Protects 'stop_threads'

    };

}

#endif // H_SCHEDULER
