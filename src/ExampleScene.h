#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "camera.h"
#include "Renderable.h"
#include "Effect.h"
#include <vector> 

struct DirLight {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
    glm::vec3 direction;
    DirLight(const glm::vec4& p_ambient, const glm::vec4& p_diffuse, const glm::vec4& p_specular, const glm::vec3& p_directional)
        : ambient(p_ambient), diffuse(p_diffuse), specular(p_specular), direction(p_directional) {}
};

struct PointLight {
	glm::vec3 position;
	float range;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

class ExampleScene: public Sample
{
public:
    ExampleScene(wolf::App* pApp);
    ~ExampleScene();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;

private:
    Camera* cam = nullptr;
    
    wolf::App* pApp = nullptr;
    std::vector<Renderable*> renderables;
    std::vector<Effect*> effects;
    DirLight m_directionalLight;
    bool m_lastDown = false;
    bool fireActive = true;
    bool stopEmitters = false;
    Effect* fireEmitter;
    Effect* fogEmitter;
};
