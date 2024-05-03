#include "Plane.h"

class SingleMaterialProvider : public wolf::Model::MaterialProvider
{
public:
    SingleMaterialProvider(const std::string& matName) : m_matName(matName) { }

    wolf::Material* getMaterial(const std::string& nodeName, int subMeshIndex, const std::string& name) const override
    {
        // Regardless of what mesh index or mat name the model wants, we just
        // use the mat we were seeded with. Note that we create a new one each
        // time so the DestroyMaterial calls line up. This could be improved,
        // but they do share shaders.
        return wolf::MaterialManager::CreateMaterial(m_matName);
    }

private:
    std::string m_matName;
};

Plane::Plane() {
    // Only init if not already done
    if(!pModel)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        pTexture = wolf::TextureManager::CreateTexture("data/GroundGrassGreen.tga");
        normalMapTexture = wolf::TextureManager::CreateTexture("data/GroundGrassGreen_normal.tga");
        
        pTexture->SetFilterMode(wolf::Texture::FM_Nearest, wolf::Texture::FM_Nearest);
        pTexture->SetWrapMode(wolf::Texture::WM_Repeat);

        normalMapTexture->SetFilterMode(wolf::Texture::FM_Nearest, wolf::Texture::FM_Nearest);
        normalMapTexture->SetWrapMode(wolf::Texture::WM_Repeat);

        const std::string MATNAME = "box";
        m_mat = wolf::MaterialManager::CreateMaterial(MATNAME);
        m_mat->SetProgram("data/textured.vsh", "data/textured.fsh");
        m_mat->SetDepthTest(true);
        m_mat->SetDepthWrite(true);   
        m_mat->SetTexture("tex1", pTexture);

        SingleMaterialProvider matProvider(MATNAME);

        pModel = new wolf::Model("data/plane.fbx", matProvider);
    }
}

Plane::~Plane() {
    wolf::MaterialManager::DestroyMaterial(m_mat);
    wolf::TextureManager::DestroyTexture(pTexture);
    delete pModel;
}

void Plane::Translate(glm::vec3 p_translation) {
    // Accumulate translation
    m_translation += p_translation;
}

void Plane::Scale(glm::vec3 p_scale) {
    // Accumulate scaling
    m_scaling *= p_scale;
}

void Plane::Rotate(glm::vec3 p_rotation) {
    // Accumulate rotation
    m_rotation += p_rotation;
}

void Plane::SetTranslate(glm::vec3 p_translation) {
    m_translation = p_translation;
}

void Plane::SetRotate(glm::vec3 p_rotation) {
    // Set rotation
    m_rotation = p_rotation;
}

void Plane::SetScale(glm::vec3 p_scale) {
    // Set scale
    m_scaling = p_scale;
}


void Plane::Render(glm::mat4 p_view, glm::mat4 p_proj) {
    glm::mat4 world = glm::translate(glm::mat4(1.0f), m_translation);
    world = glm::scale(world, m_scaling);

    pModel->Render(world, p_view, p_proj);
}

void Plane::Update(float dt) {

}