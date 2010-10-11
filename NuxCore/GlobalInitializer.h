/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef NGLOBALINITIALIZER_H
#define NGLOBALINITIALIZER_H

#ifdef _WIN32
#define NUX_GLOBAL_OBJECT_INIT_SEQUENCE()                   \
        NUX_GLOBAL_OBJECT_VARIABLE(NGlobalData);                \
        NUX_GLOBAL_OBJECT_VARIABLE(NCPU);                       \
        NUX_GLOBAL_OBJECT_VARIABLE(NProcess);                   \
        NUX_GLOBAL_OBJECT_VARIABLE(NNullOutput);                \
        NUX_GLOBAL_OBJECT_VARIABLE(UniqueIndex);               \
        NUX_GLOBAL_OBJECT_VARIABLE(NFileManagerWindows);        \
        NUX_GLOBAL_OBJECT_VARIABLE(NOutputVisualDebugConsole);  \
        NUX_GLOBAL_OBJECT_VARIABLE(NOutputLogFile);             \
        NUX_GLOBAL_OBJECT_VARIABLE(LogOutputRedirector);
//NUX_GLOBAL_OBJECT_VARIABLE(NDefaultMemoryAllocator);
//NUX_GLOBAL_OBJECT_VARIABLE(MemHook);

#elif (defined NUX_OS_LINUX)
#define NUX_GLOBAL_OBJECT_INIT_SEQUENCE()                   \
        NUX_GLOBAL_OBJECT_VARIABLE(NGlobalData);                \
        NUX_GLOBAL_OBJECT_VARIABLE(NNullOutput);                \
        NUX_GLOBAL_OBJECT_VARIABLE(UniqueIndex);               \
        NUX_GLOBAL_OBJECT_VARIABLE(NFileManagerGNU);            \
        NUX_GLOBAL_OBJECT_VARIABLE(NOutputVisualDebugConsole);  \
        NUX_GLOBAL_OBJECT_VARIABLE(NOutputLogFile);             \
        NUX_GLOBAL_OBJECT_VARIABLE(LogOutputRedirector);
//NUX_GLOBAL_OBJECT_VARIABLE(NDefaultMemoryAllocator);
//NUX_GLOBAL_OBJECT_VARIABLE(MemHook);
#endif

namespace nux
{

// This class initialize all inalogic singleton (global objects) in order. It also initialize memory allocators.
// Therefore, do not call new GlobalSingletonInitializer as it will try to call inalogic memory allocator and fail.
// You may use the global placement new operator(it is not overwritten by inalogic) to create GlobalSingletonInitializer
// inside the application data space by calling SystemInitializer(). At shutdown, call SystemShutdown()
//
// You may also create GlobalSingletonInitializer in this way:
//      main()
//      {
//          GlobalSingletonInitializer GlobalInitializer;
//      }
//
//

  class GlobalSingletonInitializer
  {
    NUX_DISABLE_OBJECT_COPY (GlobalSingletonInitializer);
    GlobalSingletonInitializer *operator & ();
    const GlobalSingletonInitializer *operator & () const;

  public:
    GlobalSingletonInitializer();
    ~GlobalSingletonInitializer();

    static bool Ready();
  private:
    static bool m_bGlobalObjectsReady;

    NUX_GLOBAL_OBJECT_INIT_SEQUENCE();
  };

// Hide these functions
// extern NUX_DLL_ENTRY void SystemStart();
// extern NUX_DLL_ENTRY void SystemShutdown();



// Nifty Counter idiom. See http://www-d0.fnal.gov/KAI/doc/tutorials/static_initialization.html
  class GlobalInitializer
  {
  public:
    GlobalInitializer();
    ~GlobalInitializer();
  private:
    static int m_Count;
  };

// Every compilation unit that includes this file will have its own instance of sGlobalInitializer. sGlobalInitializer is initialized
// before the main function of the program is called. The first time sGlobalInitializer is initialized, it calls SystemStart() to create
// our global object singleton. In SystemStart() we have a change to create our singletons in any order we like.
// When the program exits, every instance of sGlobalInitializer will be destroyed. The last instance destroyed will call SystemShutdown().
// In SystemShutdown() we can destroy our global objects in any order we like.

  static GlobalInitializer sGlobalInitializer;


}

#endif // NGLOBALINITIALIZER_H

