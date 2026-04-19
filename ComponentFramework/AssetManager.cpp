#include "AssetManager.h"

AssetManager::~AssetManager()
= default;

AssetManager& AssetManager::GetInstance()
{
    static AssetManager instance;
    return instance;
}
