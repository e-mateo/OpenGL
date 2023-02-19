#include <Scene/Light.h>


namespace LowRenderer
{
	int Light::nbDirLight;
	int Light::nbPointLight;
	int Light::nbSpotLight;

	Light::Light(const maths::Vec3 ambiente, const maths::Vec3 diffuse, const maths::Vec3 specular)
	{
		ambienteColor = ambiente;
		diffuseColor = diffuse;
		specularColor = specular;
	}

	void Light::CreateDirectionalLight(const maths::Vec3 direction, const int shaderProgram)
	{
		std::string i = std::to_string(nbDirLight);
		glUniform3f(glGetUniformLocation(shaderProgram, ("dirLights[" + i + "].direction").c_str()), direction.x, direction.y, direction.z);

		glUniform3f(glGetUniformLocation(shaderProgram, ("dirLights[" + i + "].ambient").c_str()), ambienteColor.x, ambienteColor.y, ambienteColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("dirLights[" + i + "].diffuse").c_str()), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("dirLights[" + i + "].specular").c_str()), specularColor.x, specularColor.y, specularColor.z);

		nbDirLight++;
	}

	void Light::CreatePointLight(const maths::Vec3 position, const float constant, const float linear, const float quadratic, const int shaderProgram)
	{
		std::string i = std::to_string(nbPointLight);
		glUniform3f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].position").c_str()), position.x, position.y, position.z);

		glUniform3f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].ambient").c_str()), ambienteColor.x, ambienteColor.y, ambienteColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].diffuse").c_str()), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].specular").c_str()), specularColor.x, specularColor.y, specularColor.z);

		glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(shaderProgram, ("pointLights[" + i + "].quadratic").c_str()), quadratic);

		nbPointLight++;

	}

	void Light::CreateSpotLight(const maths::Vec3 position, const maths::Vec3 direction, const float inerAngle, const float outerAngle, const int shaderProgram)
	{
		std::string i = std::to_string(nbSpotLight);
		glUniform3f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].position").c_str()), position.x, position.y, position.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].direction").c_str()), direction.x, direction.y, direction.z);

		glUniform3f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].ambient").c_str()), ambienteColor.x, ambienteColor.y, ambienteColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].diffuse").c_str()), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].specular").c_str()), specularColor.x, specularColor.y, specularColor.z);

		glUniform1f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].cosInerAngle").c_str()), cos(maths::rad(inerAngle)));
		glUniform1f(glGetUniformLocation(shaderProgram, ("spotLights[" + i + "].cosOuterAngle").c_str()), cos(maths::rad(outerAngle)));

		nbSpotLight++;
	}

}