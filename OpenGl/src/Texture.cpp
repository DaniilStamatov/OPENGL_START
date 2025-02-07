#include "Texture.h"
#include "vendor/stb_image/stb_image.h"
Texture::Texture(std::string filePath) :
	m_RendererID(0), m_Height(0), m_Width(0), m_BPP(0), m_localBuffer(nullptr), m_FilePath(filePath)
{
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	GlCall(glGenTextures(1, &m_RendererID));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
	GlCall(glBindTexture(GL_TEXTURE_2D, 0));
	

	if (m_localBuffer) 
	{
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture()
{
	GlCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot /*=0*/) const
{

	GlCall(glActiveTexture(GL_TEXTURE0 + slot));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

}

void Texture::Unbind() const
{
	GlCall(glBindTexture(GL_TEXTURE_2D, 0));
}