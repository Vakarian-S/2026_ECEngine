#include "AssetManager.h"
#include "../tinyxml/tinyxml2.h"
using namespace tinyxml2;


struct Vec3
{
    float x, y, z;

    void print()
    {
        printf("%f %f %f\n", x, y, z);
    }
};

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

bool AssetManager::ReadManifest(const char* filename)
{
    std::cout << "ReadManifest\n";
    Color4 diffuse;
    XMLDocument doc;
    doc.LoadFile(filename);
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
            for (XMLElement* assetElement = e->FirstChildElement(); assetElement != nullptr; assetElement = assetElement->NextSiblingElement())
            {
                std::cout << "Element [" << assetElement->Name() << ": " << assetElement->Value() << "] ";
                
                if (std::string_view(assetElement->Name()) == "Mesh")
                {
                    continue;
                }
                
                if (std::string_view(assetElement->Name()) == "Shader")
                {
                    continue;
                }
                
                if (std::string_view(assetElement->Name()) == "Texture")
                {
                    continue;
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
}
