#ifndef AFFECTOR_H
#define AFFECTOR_H
#include "../wolf/wolf.h"

struct Particle
{
    Particle* prev;
    Particle* next;
    int num;
    glm::vec4 color = glm::vec4(1.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 vel = glm::vec3(0.0f, 0.1f, 0.0f);
    glm::vec3 start_vel = glm::vec3(0.0f, 0.1f, 0.0f);
    float size = 1.0f;
    float start_size = 1.0f;
    float duration = -1;
    float lifetime = 0;
};

enum FunctionType {
    LINEAR, 
    PARABOLIC
};

class Affector {
public:
    // Constructor and destructor
    Affector() = default;
    virtual ~Affector() = default;

    // Virtual function to be implemented by derived classes
    virtual void AffectParticle(Particle* particle, float dt) = 0;
};

class ScaleAffector : public Affector {
public:
    ScaleAffector(float p_start, float p_end, FunctionType pType=LINEAR) : m_start(p_start), m_end(p_end), fType(pType) {}

    void AffectParticle(Particle* particle, float dt) override;

private:
    float m_start;
    float m_end; 
    FunctionType fType = LINEAR;
};


class FadeAffector : public Affector {
public:
    FadeAffector(float p_start, float p_end, FunctionType pType) : m_start(p_start), m_end(p_end), fType(pType) {}

    void AffectParticle(Particle* particle, float dt) override;

private:
    float m_start;
    float m_end; 
    FunctionType fType;
};

class VelocityAffector : public Affector {
public:
    VelocityAffector(glm::vec3 p_start, glm::vec3 p_end, FunctionType pType=LINEAR) : m_start(p_start), m_end(p_end), fType(pType) {}

    void AffectParticle(Particle* particle, float dt) override;

private:
    glm::vec3 m_start;
    glm::vec3 m_end; 
    FunctionType fType = LINEAR;
};


#endif // AFFECTOR_H
