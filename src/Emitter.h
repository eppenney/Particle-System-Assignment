#pragma once
#include "../wolf/wolf.h"
#include "tinyxml2/tinyxml2.h"
#include "Affector.h"
using namespace tinyxml2;

/*
If using xml, in the root, the following attributes are available:
num_particles - int - the total number of particles in the system
duration - float - the total duration of the particle system 
type - "continuous" or "burst" - the type of spawning 
Continuous:
    - random_birthrate - bool - whether or not the birthrate of particles should be random
    random_birthrate = true:
        - birthrate_min - float - minimum particles to spawn
        - birthrate_max - float - maximum particles to spawn 
    random_birthrate = false:
        - birthrate - float - # of particles to spawn 
Burst:
    - random_burstrate - bool - whether or not the rate of bursts is random 
    random_burstrate = true 
        - burstrate_min - float - minimum interval between bursts 
        - burstrate_max - float - maximum interval between bursts 
    random_burstrate = false
        - burstrate - float - interval between bursts 
    - random_burst_spawn_rate - bool - whether or not the number of particles per burst is random
    random_burst_spawn_rate = true
        - burst_spawn_rate_min - float - minimum particles per burst 
        - burst_spawn_rate_max - float - maximum particles per burst
    random_burst_spawn_rate = false
        - burst_spawn_rate - float - # of particles per burst 
    - repeat_burst - should this burst loop

Attributes:
- velocity - vec3 - type = random/constant - min = vec3 - max = vec3 - value = vec3
- color - vec4 - type random/constant - min = vec4 - max = vec4 - value = vec4
- size - vec3 - type = random/constant - min = vec3 - max = vec3 - value = vec3
- position - vec3 - type = point/box/sphere - min = vec3 - max = vec3 - value = vec3 - radius = float
*/

enum EmitterMode {
    CONTINUOUS,
    BURST
};

enum SpawnMode {
    POINT, 
    BOX, 
    SPHERE
};

class Emitter {
    public:
        struct Vertex {
            GLfloat x, y, z;
            GLfloat u, v; 
            GLfloat r, g, b, a;
        };

        // Class Functionality 
        Emitter();
        Emitter(std::string xml_path);
        ~Emitter();

        void Init();

        void Update(float dt);
        void Render(glm::mat4 p_view, glm::mat4 p_proj);

        void GenerateBuffers();

        // Particle System Management 
        void AddToFree(Particle* p);
        void AddToActive(Particle* p);

        Particle* CreateNewParticle(Particle* p_oldParticle = nullptr);

        void RemoveFromActive(Particle* p);

        Particle* GetFreeParticle();
        void SpawnParticle();

        void ParticleKilled(Particle* p);
        void KillParticles();

        void CalcBurstTime();

        void UpdateSpawning(float dt);
        void UpdateParticles(float dt);

        void SetOffset(glm::vec3 offset) { m_emitterPosition = offset; };
        glm::vec3 GetOffset() { return m_emitterPosition; };
    
    private:
        Particle* m_pFreeList = nullptr;
        Particle* m_pActiveListHead = nullptr;
        Particle* m_pActiveListTail = nullptr;
        EmitterMode m_mode = CONTINUOUS;
        std::vector<Affector*> affectors;

        // Emitter Settings
        int num_particles = 100;
        float m_toSpawn_accumulator;
        float m_lifetime = 0;
        float m_duration = -1;
        glm::vec3 m_emitterPosition = glm::vec3(0.0f);
        bool m_additiveBlending = false;

        // Continuous Settings 
        float m_birthRate = 10;
        bool m_bRandomBirthRate = false;
        float m_birthRateMin = 0;
        float m_birthRateMax = 4;
        
        // Burst Settings 
        float m_burstRate = 5;
        bool m_bRandomBurstRate = false;
        float m_burstRateMin = 5;
        float m_burstRateMax = 10;

        float m_burstSpawns = 3;
        bool m_bRandomBurstSpawns = false;
        float m_minBurstSpawns = 3;
        float m_maxBurstSpawns = 8;

        float m_burstTimer = 2;
        bool m_repeatBursts = false;

        // Position Settings 
        SpawnMode m_spawnShape = BOX;
        glm::vec3 m_minPosition = glm::vec3(-1.0f, -1.0f, -1.0f);
        glm::vec3 m_maxPosition = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_pos = glm::vec3(0.0f);
        float m_spawnRadius = 2.0f;

        // Color Settings
        bool m_colorRandom = false;
        glm::vec4 m_minColorValues = glm::vec4(0.0f);
        glm::vec4 m_maxColorValues = glm::vec4(1.0f);
        glm::vec4 m_color = glm::vec4(1.0f);

        // Velocity Settings 
        bool m_velocityRandom = false;
        glm::vec3 m_minVelocity = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 m_maxVelocity = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 m_velocity = glm::vec3(0.0f, 1.0f, 0.0f);

        // Particle Size Settings
        bool m_sizeRandom = false;
        float m_minSize = 0.0f;
        float m_maxSize = 1.0f;
        float m_size = 1.0f;

        // Particle lifetime settings 
        bool m_lifetimeRandom = false;
        float m_minParticleLifetime = 0.5f;
        float m_maxParticleLifetime = 5.0f;
        float m_particleLifetime = -1.0f;

        // Rendering Stuff 
        wolf::Material* m_mat = nullptr;
        wolf::Texture* m_texture = nullptr;
		GLuint m_pVertexBuffer = 0;
        GLuint m_pVertexArray = 0;
        GLuint m_pProgram = 0;
        Vertex* m_planeVertices;
};