#pragma once

#include "Emitter.h"
#include <vector> 

enum State {
    PLAYING,
    STOPPED,
    PAUSED
};

class Effect {
    public: 
        Effect();
        Effect(std::string path, glm::vec3 p_position);
        ~Effect();
        void Update(float dt);
        void Render(glm::mat4 p_view, glm::mat4 p_proj);
        void Init(std::string path, glm::vec3 p_position);
        void SetTransform(const glm::mat4& p_transform);
        void Play();
        void Stop();
        void Pause();
        glm::vec3 GetPosition() { return m_position; }; 
        void SetPosition(glm::vec3 p_position);
    private:
        std::vector<Emitter*> emitters;
        glm::mat4 m_transform;
        glm::vec3 m_position = glm::vec3(0.0f);
        State m_state = STOPPED;
        std::string m_path;
};