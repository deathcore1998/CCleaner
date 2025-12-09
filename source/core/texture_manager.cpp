#include "texture_manager.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common/constants.hpp"
#include "utils/filesystem.hpp"


core::TextureManager::TextureManager()
{
    loadAllIcons();
}

core::TextureManager::~TextureManager()
{
    clear();
}

void core::TextureManager::loadAllIcons()
{
    struct
    {
        std::string_view name;
        std::string_view path;
    } icons[] =
    {
        { common::GOOGLE_CHROME, "chrome.png" },
        { common::MICROSOFT_EDGE, "edge.png" },
        { common::MOZILLA_FIREFOX, "firefox.png" },
        { common::OPERA, "opera.png" },
        { common::YANDEX_BROWSER, "yandex.png" },

        { common::TEMP, "temp.png"},
        { common::SYSTEM, "system.png"},
    };


    const utils::FileSystem& fs = utils::FileSystem::instance();
    const fs::path iconsDir = fs.getProjectSourceDir() / "icons";

    for ( const auto& icon : icons )
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        const fs::path iconPath = iconsDir / icon.path;
        unsigned char* data = stbi_load( iconPath.string().c_str(), &width, &height, &channels, 4);

        if ( !data )
        {
            continue;
        }

        GLuint textureID;
        glGenTextures( 1, &textureID );
        glBindTexture( GL_TEXTURE_2D, textureID );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

        stbi_image_free( data );

        textures[ icon.name.data() ] = textureID;
    }
}

unsigned int core::TextureManager::getTexture( const std::string& name )
{
    auto it = textures.find( name );
    if ( it == textures.end() )
    {
        return 0;
    }
    return it->second;
}

void core::TextureManager::clear()
{
    for ( auto& pair : textures )
    {
        glDeleteTextures( 1, &pair.second );
    }
    textures.clear();
}