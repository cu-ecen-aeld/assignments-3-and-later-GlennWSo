#include "threading.h"
#include "pthread.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{


    sleep(1); // wait
    struct thread_data* data = (struct thread_data *) thread_param;    
    pthread_mutex_lock(&data->mutex); // exlusive lock
    sleep(1); // wait
    pthread_mutex_unlock(&data->mutex);



    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
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

    
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL );

    struct thread_data data = {
        .mutex =  lock,
        .wait_obtain = wait_to_obtain_ms,
        .wait_release = wait_to_release_ms,
        .thread_complete_success =  false,
    };
    int res =pthread_create(thread, NULL, &threadfunc, &data);
    if (res==0) {
        return true;
    }
    return false;
}

