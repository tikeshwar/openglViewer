#pragma once

namespace glv
{
	class Material
	{
	public:
		Material();
		Material(const char* matName);
		Material(const glm::vec3 & color);
		Material(const char* matName, const glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, float shininess = 5.0f);
		~Material();

		void setAmbientColor(const glm::vec3 & ambientColor);
		void setDiffuseColor(const glm::vec3 & diffuseColor);
		void setSpecularColor(const glm::vec3 & specularColor);
		void setShininessColor(float shininess);

		const glm::vec3 & ambientColor()const;
		glm::vec3 ambientColor();
		const glm::vec3 & diffuseColor()const;
		glm::vec3 diffuseColor();
		const glm::vec3 & specularColor()const;
		glm::vec3 specularColor();
		float shininess()const;
		float shininess();

		void applyTexture(const char* imagePath);
		GLuint textureID();
		GLuint textureID()const;
	private:
		glm::vec3 mAmbientColor;
		glm::vec3 mDiffuseColor;
		glm::vec3 mSpecularColor;
		float mShininess;
		std::string mName;
		GLuint mTextureID;
		glv::TextureLoader mTexLoader;
	};
}
