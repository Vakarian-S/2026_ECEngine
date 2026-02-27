#pragma once
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>
#include <string>

class MemoryDiagnostics
{
public:
    /**
     * Enables CRTDEBUG (Windows-specific) for detecting memory leaks at program exit
     * Call this at the start of main()
     */
    static void EnableDebugMemoryTracking()
    {
#ifdef _DEBUG
        // Enable heap checking
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        // Set a breakpoint on allocation failures
        // _CrtSetBreakAlloc(0); // Change 0 to allocation number to debug specific leak
#endif
    }

    /**
     * Reports all current allocations to the output
     * Call this before scene destruction to get a baseline
     */
    static void DumpMemoryState(const std::string& label = "")
    {
#ifdef _DEBUG
        std::cout << "\n=== Memory Dump: " << label << " ===" << std::endl;
        _CrtMemState state;
        _CrtMemCheckpoint(&state);
        _CrtMemDumpStatistics(&state);
        std::cout << "===================================\n" << std::endl;
#endif
    }

    /**
     * Compares memory states before and after an operation
     * Returns true if memory was properly freed
     */
    static bool CheckMemoryLeaks(const std::string& operationName = "")
    {
#ifdef _DEBUG
        static _CrtMemState startState;
        static bool hasStartState = false;

        if (!hasStartState)
        {
            _CrtMemCheckpoint(&startState);
            hasStartState = true;
            std::cout << "\n[MemoryDiagnostics] Start checkpoint set for: " << operationName << std::endl;
            return true;
        }

        _CrtMemState endState, diffState;
        _CrtMemCheckpoint(&endState);

        if (_CrtMemDifference(&diffState, &startState, &endState))
        {
            std::cout << "\n!!! MEMORY LEAK DETECTED !!!" << std::endl;
            std::cout << "Operation: " << operationName << std::endl;
            _CrtMemDumpStatistics(&diffState);
            return false;
        }
        else
        {
            std::cout << "\n[MemoryDiagnostics] ✓ No memory leaks detected after: " << operationName << std::endl;
            return true;
        }
#else
        std::cout << "[MemoryDiagnostics] Memory tracking only available in DEBUG mode" << std::endl;
        return true;
#endif
    }

    /**
     * Template helper to track shared_ptr reference counts
     * Useful for debugging circular references
     */
    template <typename T>
    static void PrintRefCount(const std::shared_ptr<T>& ptr, const std::string& name = "")
    {
        if (!name.empty())
            std::cout << "[RefCount] " << name << ": ";
        else
            std::cout << "[RefCount] " << typeid(T).name() << ": ";

        if (ptr)
            std::cout << "Use count = " << ptr.use_count() << std::endl;
        else
            std::cout << "nullptr" << std::endl;
    }

    /**
     * Print ref counts for multiple objects
     */
    template <typename T>
    static void PrintRefCounts(const std::vector<std::shared_ptr<T>>& ptrs, const std::string& containerName = "")
    {
        std::cout << "\n[RefCounts] " << containerName << " (" << ptrs.size() << " items):" << std::endl;
        for (size_t i = 0; i < ptrs.size(); ++i)
        {
            std::cout << "  [" << i << "] use_count = " << (ptrs[i] ? ptrs[i].use_count() : 0) << std::endl;
        }
    }

    /**
     * Call this before destroying scene to verify all actors will be freed
     */
};

