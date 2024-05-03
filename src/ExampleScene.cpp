#include "ExampleScene.h"
#include "Plane.h"
#include <cstdlib>

ExampleScene::~ExampleScene()
{
    delete cam;
    for (Renderable* renderable : renderables) {
        delete renderable;
    }
    for (Effect* effect : effects) {
        delete effect;
    }
}

ExampleScene::ExampleScene(wolf::App* tempApp) : Sample(tempApp,"ExampleScene"), 
    m_directionalLight(
        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
        glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), 
        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
        glm::vec3(0.0f, 0.0f, 1.0f)
    ) 
{
    pApp = tempApp;
}

void ExampleScene::init()
{
	// Only init if not already done
    if(!cam)
    {
        cam = new Camera(m_pApp);

        fireEmitter = new Effect("src/campfire.xml", glm::vec3(50.0f, -10.0f, -150.0f));
        fogEmitter = new Effect("src/low_fog.xml", glm::vec3(150.0f, -10.0f, -50.0f));
        

        Plane* p_plane = new Plane();
        p_plane -> Scale(glm::vec3(1.0f, 1.0f, 1.0f));
        p_plane -> Translate(glm::vec3(100.0f, -20.0f, -100.0f));

        renderables.push_back(p_plane);
        effects.push_back(fireEmitter);
        effects.push_back(fogEmitter);
        fireEmitter->Play();
    }
}

void ExampleScene::update(float dt) 
{
    if (m_pApp -> isKeyDown(' ')) {
        m_lastDown = true;
    }
    else if(m_lastDown)
    {        
        fireActive = !fireActive;
        if (fireActive) {
            stopEmitters = !stopEmitters;
            fireEmitter -> Play();
            if (stopEmitters) {
                fogEmitter -> Stop();
            } else {
                fogEmitter -> Pause();
            }
            
        } else {
            if (stopEmitters) {
                printf("Stopping Fire Emitter");
                fireEmitter -> Stop();
            } else {
                fireEmitter -> Pause();
            }
            fogEmitter -> Play();
        }
        m_lastDown = false;
    }
    cam -> update(dt);

    for (Renderable* renderable : renderables) {   
        renderable -> Update(dt);
    }

    for (Effect* effect : effects) {   
        effect -> Update(dt);
    }
}

bool compareEffectDepth(Effect& effect1, Effect& effect2, const glm::vec3& cameraPosition) {
    float distance1 = glm::distance(effect1.GetPosition(), cameraPosition);
    float distance2 = glm::distance(effect2.GetPosition(), cameraPosition);
    
    // Compare the distances
    return distance1 > distance2;
}

void ExampleScene::render(int width, int height)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 mProj = cam->getProjMatrix(width, height);
	glm::mat4 mView = cam->getViewMatrix();

    for (Renderable* renderable : renderables) {   
        wolf::Material* pMat = renderable->GetMaterial(); 
        if (!pMat) 
            continue;

        pMat->SetUniform("dirLight.ambient", m_directionalLight.ambient);
        pMat->SetUniform("dirLight.diffuse", m_directionalLight.diffuse);
        pMat->SetUniform("dirLight.specular", m_directionalLight.specular);
        pMat->SetUniform("dirLight.direction", m_directionalLight.direction);

        int num_lights = 0;

        for (int i = 0; i < num_lights; i++) {
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].position", glm::vec3(10.0f, -10.0f, 10.0f));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].ambient", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].diffuse", glm::vec4(0.0f, 0.0f, 0.75f, 1.0f));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].specular", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].range", 100.0f);
        }
        for (int i = std::min(num_lights, 4); i < 4; i++) {
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].position", glm::vec3(0.0, 0.0, 0.0));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].ambient", glm::vec4(0.0, 0.0, 0.0, 0.0));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].diffuse", glm::vec4(0.0, 0.0, 0.0, 0.0));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].specular", glm::vec4(0.0, 0.0, 0.0, 0.0));
            pMat->SetUniform("pointLights[" + std::to_string(i) + "].range", 0.0f);
        }
        renderable -> Render(mView, mProj);
    }

    std::sort(effects.begin(), effects.end(), [&](Effect* a, Effect* b) {
        return compareEffectDepth(*a, *b, cam->GetPosition());
    });

    for (Effect* effect : effects) {  
        effect -> Render(mView, mProj);
    }
}


