#pragma once
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>
#include <string>
#include "structs/Firework.h"

class MemoryDiagnostics
{
public:
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
     * Print ref counts for fireworks and their embedded light actors
     * Extracts light_actor pointers without creating new shared_ptr copies (no ref count increase)
     */
    static void PrintRefCountsForFireworks(const std::vector<std::shared_ptr<Firework>>& fireworks, const std::string& containerName = "")
    {
        std::cout << "\n[RefCounts] " << containerName << " (" << fireworks.size() << " items):" << std::endl;
        for (size_t i = 0; i < fireworks.size(); ++i)
        {
            if (fireworks[i] && fireworks[i]->light_actor)
            {
                std::cout << "  [" << i << "] use_count = " << fireworks[i]->light_actor.use_count() << std::endl;
            }
            else
            {
                std::cout << "  [" << i << "] light_actor is nullptr" << std::endl;
            }
        }
    }
};

