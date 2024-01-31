#include "threading.h"
#include "pthread.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
// #define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data* data = (struct thread_data *) thread_param;    
    data -> thread_complete_success = false; 
    bool failed = false;


    DEBUG_LOG("started sleeping before lock: %d", data -> wait_obtain);
    DEBUG_LOG("data.wait_release: %d", data -> wait_release);
    int sleep = data->wait_obtain;
    DEBUG_LOG("sleep: %d", sleep);
    usleep(data->wait_obtain); 
    failed = pthread_mutex_lock(data->mutex) != 0; 
    if (failed ) {
        ERROR_LOG("lock failed");
        return thread_param;
    };

    usleep(data->wait_release); 
    failed = pthread_mutex_unlock(data->mutex) != 0;
    if (failed ) {
        ERROR_LOG("unlock failed");
        return thread_param;
    }
    data ->thread_complete_success = true; 

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{

    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    
    // pthread_mutex_t lock;
    if (pthread_mutex_init(mutex, NULL) == -1) {
        ERROR_LOG("init mutex failed");
        return false;   
    };
    

    struct thread_data *data_ptr = malloc(sizeof(struct thread_data));
    // struct thread_data data = *data_ptr;
    data_ptr -> mutex =  mutex;
    data_ptr -> wait_obtain = wait_to_obtain_ms * 1000;
    data_ptr -> wait_release = wait_to_release_ms * 1000;
    data_ptr -> thread_complete_success =  false;

    int res =pthread_create(thread, NULL, &threadfunc, data_ptr);
    if (res!=0) {
        ERROR_LOG("thread creation failed");
        return false;
    }
    DEBUG_LOG("tread created with id: %lu", *thread);
    return true;
}

