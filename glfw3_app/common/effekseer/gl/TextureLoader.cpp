
/// #ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include "RendererImplemented.h"
#include "TextureLoader.h"
#include "GLExtension.h"
#include "effekseer/common/CommonUtils.h"
#include "effekseer/common/PngTextureLoader.h"

#include <iostream>
#include "utils/string_utils.hpp"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader( ::Effekseer::FileInterface* fileInterface )
	: m_fileInterface	( fileInterface )
{
	if( m_fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* TextureLoader::Load( const EFK_CHAR* path )
{
	std::string s;
	utils::utf16_to_utf8(path, s);
	std::cout << "Texture: '" << s << "'" << std::endl;

	std::unique_ptr<Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read( data_texture, size_texture );
		EffekseerRenderer::PngTextureLoader::Load( data_texture, size_texture, false);
		delete [] data_texture;
		std::cout << "PNG: " << static_cast<int>(size_texture) << std::endl;
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGBA,
			EffekseerRenderer::PngTextureLoader::GetWidth(), 
			EffekseerRenderer::PngTextureLoader::GetHeight(),
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			EffekseerRenderer::PngTextureLoader::GetData().data());
		
		/* �~�b�v�}�b�v�̐��� */
		GLExt::glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return reinterpret_cast<void*>(texture);
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	if( data != NULL )
	{
		GLuint texture = EffekseerRenderer::TexturePointerToTexture <GLuint> (data);
		glDeleteTextures(1, &texture);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/// #endif
