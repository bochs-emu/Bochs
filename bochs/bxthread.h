/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2017-2020  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#ifndef BX_THREAD_H
#define BX_THREAD_H

// Bochs multi-threading support

#if BX_WITH_SDL || BX_WITH_SDL2

#include <SDL_mutex.h>
#include <SDL_timer.h>
#include <SDL_thread.h>

#define BX_THREAD_VAR(name) SDL_Thread* name = NULL
#define BX_THREAD_FUNC(name,arg) static int name(void* arg)
#define BX_THREAD_EXIT return 0
#if BX_WITH_SDL2
#define BX_THREAD_CREATE(name,arg,var) do { var = SDL_CreateThread(name, #name, (void*)arg); } while (0)
#define BX_THREAD_KILL(var) SDL_DetachThread(var)
#else
#define BX_THREAD_CREATE(name,arg,var) do { var = SDL_CreateThread(name, (void*)arg); } while (0)
#define BX_THREAD_KILL(var) SDL_KillThread(var)
#endif
#define BX_LOCK(mutex) SDL_LockMutex(mutex)
#define BX_UNLOCK(mutex) SDL_UnlockMutex(mutex)
#define BX_MUTEX(mutex) SDL_mutex *mutex
#define BX_INIT_MUTEX(mutex) mutex = SDL_CreateMutex()
#define BX_FINI_MUTEX(mutex) SDL_DestroyMutex(mutex)
#define BX_MSLEEP(val) SDL_Delay(val)

#elif defined(WIN32)

#define BX_THREAD_VAR(name) HANDLE name
#define BX_THREAD_FUNC(name,arg) DWORD WINAPI name(LPVOID arg)
#define BX_THREAD_EXIT return 0
#define BX_THREAD_CREATE(name,arg,var) do { var = CreateThread(NULL, 0, name, arg, 0, NULL); } while (0)
#define BX_THREAD_KILL(var) TerminateThread(var, 0)
#define BX_LOCK(mutex) EnterCriticalSection(&(mutex))
#define BX_UNLOCK(mutex) LeaveCriticalSection(&(mutex))
#define BX_MUTEX(mutex) CRITICAL_SECTION mutex
#define BX_INIT_MUTEX(mutex) InitializeCriticalSection(&(mutex))
#define BX_FINI_MUTEX(mutex) DeleteCriticalSection(&(mutex))
#define BX_MSLEEP(val) Sleep(val)

#else

#include <pthread.h>

#define BX_THREAD_VAR(name) pthread_t (name)
#define BX_THREAD_FUNC(name,arg) void name(void* arg)
#define BX_THREAD_EXIT pthread_exit(NULL)
#define BX_THREAD_CREATE(name,arg,var) \
    pthread_create(&(var), NULL, (void *(*)(void *))&(name), arg)
#define BX_THREAD_KILL(var) pthread_cancel(var); pthread_join(var, NULL)
#define BX_LOCK(mutex) pthread_mutex_lock(&(mutex));
#define BX_UNLOCK(mutex) pthread_mutex_unlock(&(mutex));
#define BX_MUTEX(mutex) pthread_mutex_t (mutex)
#define BX_INIT_MUTEX(mutex) pthread_mutex_init(&(mutex),NULL)
#define BX_FINI_MUTEX(mutex) pthread_mutex_destroy(&(mutex))
#define BX_MSLEEP(val) usleep(val*1000)

#endif

typedef struct
{
#if BX_WITH_SDL || BX_WITH_SDL2
  SDL_cond *cond;
  SDL_mutex *lock;  
#elif defined(WIN32)
  HANDLE event;
#else
  pthread_cond_t cond;
  pthread_mutex_t lock;
#endif
} bx_thread_event_t;

void BOCHSAPI_MSVCONLY bx_create_event(bx_thread_event_t *thread_ev);
void BOCHSAPI_MSVCONLY bx_destroy_event(bx_thread_event_t *thread_ev);
void BOCHSAPI_MSVCONLY bx_set_event(bx_thread_event_t *thread_ev);
bx_bool BOCHSAPI_MSVCONLY bx_wait_for_event(bx_thread_event_t *thread_ev);

#endif
