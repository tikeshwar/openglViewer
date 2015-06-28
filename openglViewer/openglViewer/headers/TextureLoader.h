#pragma once

namespace glv
{
	class TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader();

		GLuint loadFreeImage(const char* imagepath);
		GLuint loadBMP_custom(const char * imagepath);
		GLuint loadDDS(const char * imagepath);

	private:
		std::shared_ptr<TextureManager> mTextureManager;
	};
}
