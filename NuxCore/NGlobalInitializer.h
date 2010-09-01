#ifndef NGLOBALINITIALIZER_H
#define NGLOBALINITIALIZER_H

#ifdef _WIN32
    #define INL_GLOBAL_OBJECT_INIT_SEQUENCE()                   \
        INL_GLOBAL_OBJECT_VARIABLE(NGlobalData);                \
        INL_GLOBAL_OBJECT_VARIABLE(NCPU);                       \
        INL_GLOBAL_OBJECT_VARIABLE(NProcess);                   \
        INL_GLOBAL_OBJECT_VARIABLE(NNullOutput);                \
        INL_GLOBAL_OBJECT_VARIABLE(NUniqueIndex);               \
        INL_GLOBAL_OBJECT_VARIABLE(NFileManagerWindows);        \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputVisualDebugConsole);  \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputLogFile);             \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputDeviceRedirector);    \
        INL_GLOBAL_OBJECT_VARIABLE(NDefaultMemoryAllocator);    \
        INL_GLOBAL_OBJECT_VARIABLE(MemHook);

#elif (defined INL_PS3)
    #define INL_GLOBAL_OBJECT_INIT_SEQUENCE()                   \
        INL_GLOBAL_OBJECT_VARIABLE(NNullOutput);                \
        INL_GLOBAL_OBJECT_VARIABLE(NUniqueIndex);               \
        INL_GLOBAL_OBJECT_VARIABLE(NFileManagerPS3);            \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputLogFile);             \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputDeviceRedirector);    \
        INL_GLOBAL_OBJECT_VARIABLE(NDefaultMemoryAllocator);    \
        INL_GLOBAL_OBJECT_VARIABLE(MemHook);
#elif (defined INL_OS_LINUX)
    #define INL_GLOBAL_OBJECT_INIT_SEQUENCE()                   \
        INL_GLOBAL_OBJECT_VARIABLE(NGlobalData);                \
        INL_GLOBAL_OBJECT_VARIABLE(NNullOutput);                \
        INL_GLOBAL_OBJECT_VARIABLE(NUniqueIndex);               \
        INL_GLOBAL_OBJECT_VARIABLE(NFileManagerGNU);            \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputVisualDebugConsole);  \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputLogFile);             \
        INL_GLOBAL_OBJECT_VARIABLE(NOutputDeviceRedirector);    \
        INL_GLOBAL_OBJECT_VARIABLE(NDefaultMemoryAllocator);    \
        INL_GLOBAL_OBJECT_VARIABLE(MemHook);
#endif

NAMESPACE_BEGIN

// This class initialize all inalogic singleton (global objects) in order. It also initialize memory allocators.
// Therefore, do not call new NGlobalSingletonInitializer as it will try to call inalogic memory allocator and fail.
// You may use the global placement new operator(it is not overwritten by inalogic) to create NGlobalSingletonInitializer 
// inside the application data space by calling SystemInitializer(). At shutdown, call SystemShutdown()
//       
// You may also create NGlobalSingletonInitializer in this way:
//      main()
//      {
//          NGlobalSingletonInitializer GlobalInitializer;
//      }
//
//

class NGlobalSingletonInitializer
{
    INL_DISABLE_OBJECT_COPY(NGlobalSingletonInitializer);
    NGlobalSingletonInitializer* operator & ();
    const NGlobalSingletonInitializer* operator & () const;

public:
    NGlobalSingletonInitializer();
    ~NGlobalSingletonInitializer();

    static bool Ready();
private:
    static bool m_bGlobalObjectsReady;

    INL_GLOBAL_OBJECT_INIT_SEQUENCE();
};

// Hide these functions
// extern INL_DLL_ENTRY void SystemStart();
// extern INL_DLL_ENTRY void SystemShutdown();



// Nifty Counter idiom. See http://www-d0.fnal.gov/KAI/doc/tutorials/static_initialization.html
class NGlobalInitializer
{
public:
    NGlobalInitializer();
    ~NGlobalInitializer();
private:
    static int m_Count;
};

// Every compilation unit that includes this file will have its own instance of sGlobalInitializer. sGlobalInitializer is initialized
// before the main function of the program is called. The first time sGlobalInitializer is initialized, it calls SystemStart() to create
// our global object singleton. In SystemStart() we have a change to create our singletons in any order we like.
// When the program exits, every instance of sGlobalInitializer will be destroyed. The last instance destroyed will call SystemShutdown().
// In SystemShutdown() we can destroy our global objects in any order we like.

static NGlobalInitializer sGlobalInitializer;


NAMESPACE_END

#endif // NGLOBALINITIALIZER_H

