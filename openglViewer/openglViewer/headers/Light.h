#pragma once

namespace glv
{
	class Light
	{
	public:
		// defaults to white light with no spot component positioned at (0,0,0)
		Light(const char* name);
		// defaults to white light with no spot component positioned at position
		Light(const char* name, const glm::vec3 & position, const glm::vec3 & lookAt);
		// defaults to given light color with no spot component
		Light(const char* name, const glm::vec3 & color, const glm::vec3 & position, const glm::vec3 & lookAt);
		Light(const char* name, const glm::vec3 & ambientColor, const glm::vec3 & diffuseColor, const glm::vec3 & specularColor, const glm::vec3 & position, const glm::vec3 & lookAt);

		~Light();

		void setName(const char* name);
		void setColor(const glm::vec3 & color);
		void setPosition(const glm::vec3 & position);
		void setLookAt(const glm::vec3 & lookAt);
		void setAmbientColor(const glm::vec3 & color);
		void setDiffuseColor(const glm::vec3 & color);
		void setSpecularColor(const glm::vec3 & color);

		void setSpotComponents(const glm::vec3& spotDirection, float spotCutoff);
		void turnOnSpotLight(bool turnOn);

		const char* name();
		const char* name()const;

		const glm::vec3 & ambientColor()const;
		glm::vec3 ambientColor();
		const glm::vec3 & diffuseColor()const;
		glm::vec3 diffuseColor();
		const glm::vec3 & specularColor()const;
		glm::vec3 specularColor();

		const glm::vec3 & position()const;
		glm::vec3 position();

		const glm::vec3 & lookAt()const;
		glm::vec3 lookAt();

		const glm::vec3 & spotDirection()const;
		glm::vec3 spotDirection();

		float spotCutoff()const;
		float spotCutoff();

		float constantAttenuation()const;
		float constantAttenuation();

		float linearAttenuation()const;
		float linearAttenuation();

		float quadraticAttenuation()const;
		float quadraticAttenuation();

		bool isSpotLightOn()const;
		bool isSpotLightOn();

	private:
		glm::vec3 mAmbientColor;
		glm::vec3 mDiffuseColor;
		glm::vec3 mSpecularColor;

		glm::vec3 mPosition;
		glm::vec3 mLookAt;


		glm::vec3 mSpotDirection;
		float mSpotCutoff;
		float mConstantAttenuation;
		float mLinearAttenuation;
		float mQuadraticAttenuation;
		bool mIsSpotTurnedOn;

		std::string mName;
	};

}

