/*
 * Copyright (c) 2020 YuQing <384681@qq.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the Lesser GNU General Public License, version 3
 * or later ("LGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PTHREAD_FUNC_H
#define PTHREAD_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "common_define.h"
#include "logger.h"
#include "sched_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

int init_pthread_lock(pthread_mutex_t *pthread_lock);
int init_pthread_attr(pthread_attr_t *pattr, const int stack_size);

int init_pthread_lock_cond_pair(pthread_lock_cond_pair_t *lcp);
void destroy_pthread_lock_cond_pair(pthread_lock_cond_pair_t *lcp);

#define PTHREAD_MUTEX_LOCK(lock) \
    do {  \
        int lock_res;   \
        if ((lock_res=pthread_mutex_lock(lock)) != 0) \
        {  \
            logWarning("file: "__FILE__", line: %d, "  \
                    "call pthread_mutex_lock fail, " \
                    "errno: %d, error info: %s", \
                    __LINE__, lock_res, STRERROR(lock_res)); \
        }  \
    } while (0)


#define PTHREAD_MUTEX_UNLOCK(lock) \
    do {  \
        int unlock_res;   \
        if ((unlock_res=pthread_mutex_unlock(lock)) != 0) \
        {  \
            logWarning("file: "__FILE__", line: %d, "    \
                    "call pthread_mutex_unlock fail, " \
                    "errno: %d, error info: %s", \
                    __LINE__, unlock_res, STRERROR(unlock_res)); \
        }  \
    } while (0)


#define lcp_timedwait_sec(lcp, timeout)    \
    fc_timedwait_sec(&(lcp)->lock, &(lcp)->cond, timeout)

#define lcp_timedwait_ms(lcp, timeout_ms)  \
    fc_timedwait_ms(&(lcp)->lock, &(lcp)->cond, timeout_ms)

static inline void fc_timedwait_sec(pthread_mutex_t *lock,
        pthread_cond_t *cond, const int timeout)
{
    struct timespec ts;

    PTHREAD_MUTEX_LOCK(lock);
    ts.tv_sec = get_current_time() + timeout;
    ts.tv_nsec = 0;
    pthread_cond_timedwait(cond, lock, &ts);
    PTHREAD_MUTEX_UNLOCK(lock);
}

static inline void fc_timedwait_ms(pthread_mutex_t *lock,
        pthread_cond_t *cond, const int timeout_ms)
{
    struct timespec ts;

    PTHREAD_MUTEX_LOCK(lock);
    ts.tv_sec = get_current_time() + timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * (1000 * 1000);
    pthread_cond_timedwait(cond, lock, &ts);
    PTHREAD_MUTEX_UNLOCK(lock);
}

int create_work_threads(int *count, void *(*start_func)(void *),
		void **args, pthread_t *tids, const int stack_size);

int create_work_threads_ex(int *count, void *(*start_func)(void *),
		void *args, const int elment_size, pthread_t *tids,
        const int stack_size);

int kill_work_threads(pthread_t *tids, const int count);

int fc_create_thread(pthread_t *tid, void *(*start_func)(void *),
        void *args, const int stack_size);

#ifdef __cplusplus
}
#endif

#endif

