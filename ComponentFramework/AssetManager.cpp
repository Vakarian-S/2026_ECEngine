#include "AssetManager.h"

#include <QMath.h>
#include <ranges>

#include "Actor.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "../tinyxml/tinyxml2.h"
#include "scenes/Scene1.h"
class TransformComponent;
using namespace tinyxml2;

struct Color4
{
    float red, green, blue, alpha;

    void print()
    {
        printf("%f %f %f %f\n", red, green, blue, alpha);
    }
};

AssetManager::~AssetManager()
= default;

AssetManager& AssetManager::GetInstance()
{
    static AssetManager instance;
    return instance;
}

bool AssetManager::ReadManifest(const char* manifsetFilename)
{
    std::cout << "ReadManifest\n";
    Color4 diffuse;
    XMLDocument doc;
    doc.LoadFile(manifsetFilename);
    if (doc.Error())
    {
        std::cout << tinyxml2::XMLDocument::ErrorIDToName(doc.ErrorID()) << std::endl;
        return false;
    }
    /// Jump to the first node or "root"
    XMLElement* rootData = doc.RootElement();

    /// Loop over all the elements under the first node
    for (XMLElement* e = rootData->FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
    {
        /// Print the name of the element
        std::cout << "Element [" << e->Value() << "]: ";

        /** Read Assets First **/
        if (std::string_view(e->Value()) == "Assets")
        {
            std::cout << "Assets Found\n";
            for (XMLElement* assetElement = e->FirstChildElement(); assetElement != nullptr; assetElement = assetElement
                 ->NextSiblingElement())
            {
                std::cout << "Element [" << assetElement->Name() << ": " << assetElement->Value() << "] ";

                if (std::string_view(assetElement->Name()) == "Mesh")
                {
                    const char* name = assetElement->Attribute("name");
                    const char* filename = assetElement->Attribute("filename");
                    std::string key = std::string(asset_prefix(Asset_type::MESH)) + name;
                    std::cout << "Name: " << name << '\n';
                    std::cout << "Filename: " << filename << '\n';
                    std::cout << "Key: " << key << '\n';
                    AddComponent<MeshComponent>(key.c_str(), WeakRef<Component>(),
                                                std::string(filename).c_str());
                    continue;
                }

                if (std::string_view(assetElement->Name()) == "Shader")
                {
                    const char* name = assetElement->Attribute("name");
                    const char* filenameVert = assetElement->Attribute("filenameVert");
                    const char* filenameFrag = assetElement->Attribute("filenameFrag");
                    std::string key = std::string(asset_prefix(Asset_type::SHADER)) + name;
                    std::cout << "Name: " << name << '\n';
                    std::cout << "FilenameVer: " << filenameVert << '\n';
                    std::cout << "filenameFrag: " << filenameFrag << '\n';
                    std::cout << "Key: " << key << '\n';
                    AddComponent<ShaderComponent>(key.c_str(), WeakRef<Component>(), "shaders/texturePhongVert.glsl",
                                                  "shaders/texturePhongFrag.glsl");
                    continue;
                }

                if (std::string_view(assetElement->Name()) == "Texture")
                {
                    const char* name = assetElement->Attribute("name");
                    const char* filename = assetElement->Attribute("filename");
                    std::string key = std::string(asset_prefix(Asset_type::TEXTURE)) + name;
                    std::cout << "Name: " << name << '\n';
                    std::cout << "Filename: " << filename << '\n';
                    std::cout << "Key: " << key << '\n';
                    AddComponent<MaterialComponent>(key.c_str(), WeakRef<Component>(),
                                                    filename);
                }
            }
        }

        /** Reads Actors to put on the scene **/
        if (std::string_view(e->Value()) == "Actors")
        {
            std::cout << "Actors Found\n";
            for (XMLElement* actorElement = e->FirstChildElement(); actorElement != nullptr; actorElement = actorElement
                ->NextSiblingElement())
            {
                ActorInfo actorInfo = {};
                auto actor = std::make_shared<Actor>(WeakRef<Component>());
                auto pieceColor = "white";
                auto meshName = "Pawn";
                for (XMLElement* componentElement = actorElement->FirstChildElement(); componentElement != nullptr; componentElement = componentElement
                ->NextSiblingElement())
                {
                    std::cout << "Component [" << componentElement->Name() << ": " << componentElement->Value() << "] ";
                    if (std::string_view(componentElement->Name()) == "Mesh")
                    {
                        const char* name = componentElement->Attribute("name");
                        meshName = name;
                        std::string key = std::string(asset_prefix(Asset_type::MESH)) + name;
                        std::cout << "Key: " << key << '\n';
                        actorInfo.mesh_name = key;
                        if (auto component = GetComponent<MeshComponent>(key.c_str()))
                        {
                            actor->AddComponent(component);
                        }
                        continue;
                    }
                    if (std::string_view(componentElement->Name()) == "Shader")
                    {
                        const char* name = componentElement->Attribute("name");
                        std::string key = std::string(asset_prefix(Asset_type::SHADER)) + name;
                        std::cout << "Key: " << key << '\n';
                        actorInfo.shader_name = key;
                        if (auto component = GetComponent<ShaderComponent>(key.c_str()))
                        {
                            actor->AddComponent(component);
                        }
                        continue;
                    }
                    if (std::string_view(componentElement->Name()) == "Texture")
                    {
                        const char* name = componentElement->Attribute("name");
                        std::string key = std::string(asset_prefix(Asset_type::TEXTURE)) + name;
                        std::cout << "Key: " << key << '\n';
                        actorInfo.texture_name = key;
                        if (auto component = GetComponent<MaterialComponent>(key.c_str()))
                        {
                            actor->AddComponent(component);
                        }
                        continue;
                    }
                    if (std::string_view(componentElement->Name()) == "ChessData")
                    {
                        const char* color = componentElement->Attribute("color");
                        pieceColor = color;
                    }
                }
                
                auto chessPiece = Chess_pieces::PAWN;
                if (std::string_view(meshName) == "Pawn") chessPiece = Chess_pieces::PAWN;
                if (std::string_view(meshName) == "Knight") chessPiece = Chess_pieces::KNIGHT;
                if (std::string_view(meshName) == "Bishop") chessPiece = Chess_pieces::BISHOP;
                if (std::string_view(meshName) == "Rook") chessPiece = Chess_pieces::ROOK;
                if (std::string_view(meshName) == "Queen") chessPiece = Chess_pieces::QUEEN;
                if (std::string_view(meshName) == "King") chessPiece = Chess_pieces::KING;
                
                if (std::string_view(pieceColor) == "white")
                {
                    white_pieces_.emplace_back(std::move(actor));
                    white_pieces_map_.insert({chessPiece, actorInfo});
                } else
                {
                    black_pieces_.emplace_back(std::move(actor));
                    black_pieces_map_.insert({chessPiece, actorInfo});
                }
            }
        }

        /// If there is text within the element (not null), print it
        if (e->GetText() != nullptr)
        {
            std::cout << e->GetText() << '\n';
        }

        /// loop over all the attributes (if any) 
        for (const XMLAttribute* a = e->FirstAttribute(); a != nullptr; a = a->Next())
        {
            std::cout << "Attribute [" << a->Name() << ": " << a->Value() << "] ";
        }
        std::cout << '\n';
        
    }
    ListComponents();
}

void AssetManager::ListComponents()
{
    std::cout << "Components loaded into AssetManager:" << std::endl;
    for (auto& [key, value] : component_catalog_)
    {
        std::cout << key << ": " << value << std::endl;
    }
    std::cout << "Actors for white pieces loaded into AssetManager:" << std::endl;
    for (auto& [key, value] : white_pieces_map_)
    {
        if (key == Chess_pieces::PAWN)  std::cout <<  "Pawn : " << value.mesh_name << std::endl;
        if (key == Chess_pieces::KNIGHT) std::cout << "Knight: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::BISHOP) std::cout << "Bishop: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::ROOK)   std::cout << "Rook: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::QUEEN)  std::cout << "Queen: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::KING)   std::cout << "King: " << value.mesh_name << std::endl;   
    }
    
    std::cout << "Actors for black pieces loaded into AssetManager:" << std::endl;
    for (auto& [key, value] : black_pieces_map_)
    {
        if (key == Chess_pieces::PAWN)  std::cout <<  "Pawn : " << value.mesh_name << std::endl;
        if (key == Chess_pieces::KNIGHT) std::cout << "Knight: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::BISHOP) std::cout << "Bishop: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::ROOK)   std::cout << "Rook: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::QUEEN)  std::cout << "Queen: " << value.mesh_name << std::endl;
        if (key == Chess_pieces::KING)   std::cout << "King: " << value.mesh_name << std::endl;   
    }
}
