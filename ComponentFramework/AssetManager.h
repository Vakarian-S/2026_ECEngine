#pragma once

class AssetManager
{
private:
    /* Private default constructor.
      Prevents external instantiation */
    AssetManager() = default;

public:
    /** Singleton disabled operations **/
    /** Prevents copying **/
    AssetManager(const AssetManager&) = delete;
    /** Prevents moving **/
    AssetManager(AssetManager&&) = delete;
    /**  Prevents assignment-based duplication **/
    AssetManager& operator=(const AssetManager&) = delete;
    /** Prevents move-based reassignment and enforces controlled lifetime **/
    AssetManager& operator=(AssetManager&&) = delete;
    
    ~AssetManager();
    
    /** Singleton accessor (Meyers Singleton). **/
    static AssetManager& GetInstance();    
    
};
