#include "../wolf/wolf.h"
#ifndef RENDERABLE_H
#define RENDERABLE_H

namespace wolf {
    class Material;
}

class Renderable {
public:
    virtual ~Renderable() {}

    virtual void Update(float dt) {};

    virtual void Render(glm::mat4 p_view, glm::mat4 p_proj) {};

    virtual void SetMaterial(wolf::Material* p_mat) { m_Mat = p_mat; };
    virtual wolf::Material* GetMaterial() { return m_Mat; };

private:
    wolf::Material* m_Mat;
};

#endif // RENDERABLE_H