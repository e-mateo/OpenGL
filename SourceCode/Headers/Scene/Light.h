#pragma once
#include<String>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Maths.h>


namespace LowRenderer
{
	class Light
	{
		maths::Vec3 ambienteColor;
		maths::Vec3 diffuseColor;
		maths::Vec3 specularColor;

	public:
		Light(const maths::Vec3 ambiente, const maths::Vec3 diffuse, const maths::Vec3 specular);

		static int nbDirLight;
		static int nbSpotLight;
		static int nbPointLight;

		void CreateDirectionalLight(const maths::Vec3 direction, const int shaderProgram);
		void CreatePointLight(const maths::Vec3 position, const float constant, const float linear, const float quadratic, const int shaderProgram);
		void CreateSpotLight(const maths::Vec3 position, const maths::Vec3 direction, const float inerAngle, const float outerAngle, const int shaderProgram);

	};
}