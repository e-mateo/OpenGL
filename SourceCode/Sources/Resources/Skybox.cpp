#include <Resources/Skybox.h>


Skybox::Skybox()
{
    cubemapCartoonScene = NULL;
    cubemapSkyboxScene = NULL;
}

void Skybox::LoadCubeMap(std::vector<std::string> faces, unsigned int& cubeMap)
{
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            if(nrChannels == 3)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            if (nrChannels == 4)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::LoadSkybox()
{
    model = new Resources::Model;
	model->CreateSkybox();
	model->BindBuffer();

    std::vector<std::string> skyboxFaces
    {
        "Resources/Skybox/right.jpg",
        "Resources/Skybox/left.jpg",
        "Resources/Skybox/top.jpg",
        "Resources/Skybox/bottom.jpg",
        "Resources/Skybox/front.jpg",
        "Resources/Skybox/back.jpg"
    };


    LoadCubeMap(skyboxFaces, cubemapSkyboxScene);
}

void Skybox::Draw(LowRenderer::Camera& camera, Resources::ResourceManager& resourcesManager)
{
    int skyboxProgram = resourcesManager.Get<Resources::Shader>("skyboxShader")->shaderProgram;

    glDepthMask(GL_FALSE);
    glUseProgram(skyboxProgram);
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "viewMatrix"), 1, GL_FALSE, &camera.GetViewMatrix().matrixArray[0]);
    glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projMatrix"), 1, GL_FALSE, &camera.GetProjMatrix().matrixArray[0]);
    glBindVertexArray(model->GetVAO());
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapSkyboxScene);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{
    delete model;
}