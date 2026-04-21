#pragma once
#include <unordered_map>

#include "Actor.h"
#include "Component.h"
#include "Debug.h"
#include "scenes/Scene1.h"

enum class Asset_type : uint8_t
{
    MESH = 0,
    SHADER,
    TEXTURE
};

static const char* asset_prefix(Asset_type type)
{
    switch (type)
    {
    case Asset_type::MESH: return "mesh_";
    case Asset_type::SHADER: return "shader_";
    case Asset_type::TEXTURE: return "texture_";
    }
    return "";
}

struct ActorInfo
{
    std::string mesh_name;
    std::string shader_name;
    std::string texture_name;
};


class AssetManager
{
private:
    /* Private default constructor.
      Prevents external instantiation */
    AssetManager() = default;

    std::unordered_map<std::string, Ref<Component>> component_catalog_;
    std::unordered_map<Chess_pieces, ActorInfo> white_pieces_map_;
    std::unordered_map<Chess_pieces, ActorInfo> black_pieces_map_;

public:
    [[nodiscard]] std::unordered_map<Chess_pieces, ActorInfo> WhitePiecesMap() const
    {
        return white_pieces_map_;
    }

    [[nodiscard]] std::unordered_map<Chess_pieces, ActorInfo> BlackPiecesMap() const
    {
        return black_pieces_map_;
    }

private:
    
    
    std::vector<std::shared_ptr<Actor>> white_pieces_;

private:
    std::vector<std::shared_ptr<Actor>> black_pieces_;
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

    bool ReadManifest(const char* manifsetFilename);


    template <typename ComponentTemplate, typename... Args>
    void AddComponent(const char* name, Args&&... args_)
    {
        Ref<ComponentTemplate> t = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...);
        component_catalog_[name] = t;
    }

    template <typename ComponentTemplate>
    Ref<ComponentTemplate> GetComponent(const char* name) const
    {
        auto id = component_catalog_.find(name);
#ifdef _DEBUG
        if (id == component_catalog_.end())
        {
            Debug::Error("Can't find requested component", __FILE__, __LINE__);
            return Ref<ComponentTemplate>(nullptr);
        }
#endif
        return std::dynamic_pointer_cast<ComponentTemplate>(id->second);
    }

    /** Prints all the components from the manager **/;
    void ListComponents();
};
