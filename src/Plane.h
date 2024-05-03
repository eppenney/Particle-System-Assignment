#include "../wolf/wolf.h"
#include "Renderable.h"

class Plane : public Renderable {
    public:
        Plane();
        ~Plane();
        void Translate(glm::vec3 p_translation);
        void Rotate(glm::vec3 p_rotate);
        void Scale(glm::vec3 p_scale);
        void SetTranslate(glm::vec3 p_translation);
        void SetRotate(glm::vec3 p_rotate);
        void SetScale(glm::vec3 p_scale);

        void Render(glm::mat4 p_view, glm::mat4 p_proj) override;
        void Update(float dt) override;

        void SetMaterial(wolf::Material* p_mat) override { m_mat = p_mat; };
        wolf::Material* GetMaterial() override { return m_mat; };

    private: 
        wolf::Model* pModel = nullptr;
        wolf::Material* m_mat = nullptr;
        wolf::Texture* pTexture = nullptr;
        wolf::Texture* normalMapTexture = nullptr;
        glm::vec3 m_translation = glm::vec3(0.0f);
        glm::vec3 m_scaling = glm::vec3(1.0f);
        glm::vec3 m_rotation = glm::vec3(0.0f);
};