#include "Affector.h"

void ScaleAffector::AffectParticle(Particle* particle, float dt) {
    if (particle -> duration != -1) {
        float interpolateValue = particle -> lifetime / particle -> duration;
        float scaleFactor = m_start + (m_end - m_start) * interpolateValue;
        particle -> size = particle -> start_size * scaleFactor;
    }
    else {
        if (particle -> size < m_end) {
            particle -> size += dt;
        }
    }
}

void FadeAffector::AffectParticle(Particle* particle, float dt) {
    if (fType == LINEAR) {
        if (particle -> duration != -1) {
            float interpolateValue = (particle -> lifetime) / (particle -> duration);
            float fadeValue = m_end - m_start;
            particle -> color.a = m_start + (fadeValue * interpolateValue);
        }
        else {
            if (m_end > m_start && particle -> color.a < m_end) {
                particle -> color.a += dt;
            } else if (m_end < m_start && particle -> color.a > m_end) {
                particle -> color.a -= dt;
            }
        }   
    } else if (fType == PARABOLIC) {
        if (particle->duration != -1) {
            float interpolateValue = particle->lifetime / particle->duration;

            float parabolicValue = -4 * (interpolateValue - 0.5) * (interpolateValue - 0.5) + 1;

            // Calculate the parabolic interpolation for alpha value
            particle->color.a = m_start + (m_end - m_start) * parabolicValue;
        }
    }
}

void VelocityAffector::AffectParticle(Particle* particle, float dt) {
    if (particle -> duration != -1) {
        float interpolateValue = (particle -> lifetime) / (particle -> duration);
        glm::vec3 velocityValue = m_end - m_start;
        particle -> vel = particle -> start_vel + (velocityValue * interpolateValue * particle -> start_vel);
    }
    else {
        if (glm::length(m_end) > glm::length(m_start) && glm::length(particle -> vel) < glm::length(m_end)) {
            particle -> vel *= (1 + dt);
        } else if (glm::length(m_end) < glm::length(m_start) && glm::length(particle -> vel) > glm::length(m_end)) {
            particle -> vel *= (1 - dt);
        }
    }
    
}