#pragma once

#include <string>
#include <unordered_map>

namespace core
{
    class TextureManager
    {
    public:
        TextureManager();
        ~TextureManager();

        void loadAllIcons();
        unsigned int getTexture( const std::string& name );
        void clear();

    private:
        std::unordered_map< std::string, unsigned int > textures;
    };
}