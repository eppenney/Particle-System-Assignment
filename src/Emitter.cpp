#include "Emitter.h"
#include <random>

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


static const Emitter::Vertex planeVertices[] = {
    {-0.5f, 0.0f, -0.5f, 0.0f, 0.0f},
    {-0.5f, 0.0f, 0.5f, 0.0f, 1.0f},
    {0.5f, 0.0f, -0.5f, 1.0f, 0.0f},
    
    {0.5f, 0.0f, -0.5f, 1.0f, 0.0f},
    {-0.5f, 0.0f, 0.5f, 0.0f, 1.0f},
    {0.5f, 0.0f, 0.5f, 1.0f, 1.0f},
};

/*
Helpful Sphere stuff from ChatGPT from the prompt:
How to get random point within a sphere / within a certain distance of a point?
Assume you have access to m_spawnRadius, a float value
*/
glm::vec3 generateRandomPointInSphere(const glm::vec3& pos, float m_spawnRadius) {
    // Generate random angles
    float theta = acos(1 - 2 * static_cast<float>(rand()) / RAND_MAX);
    float phi = 2 * PI * static_cast<float>(rand()) / RAND_MAX;
    
    // Generate a random distance
    float r = m_spawnRadius * pow(static_cast<float>(rand()) / RAND_MAX, 1.0f / 3.0f);
    
    // Convert spherical coordinates to Cartesian coordinates
    float x = pos.x + r * sin(theta) * cos(phi);
    float y = pos.y + r * sin(theta) * sin(phi);
    float z = pos.z + r * cos(theta);
    
    // Return the random point as a glm::vec3
    return glm::vec3(x, y, z);
}

Emitter::Emitter() {
}

Emitter::Emitter(std::string xml_path) {
    // printf("Finding File - ");
    XMLDocument xmlDoc;

    tinyxml2::XMLError error = xmlDoc.LoadFile(xml_path.c_str());

    if (error != tinyxml2::XML_SUCCESS) {
        // printf("File Not Found\n");
        return;
    }

    // printf("File Found\nFinding Root - ");
    XMLElement *root = xmlDoc.RootElement();
    if (!root) {
        // printf("Root not found\n");
        return;
    }
    // printf("Root found\n");

    // Emitter Properties 
    if (root->QueryIntAttribute("num_particles", &num_particles) != tinyxml2::XML_SUCCESS) {
        // printf("Failed to parse num_particles - setting to default\n");
    }
    if (root->QueryFloatAttribute("duration", &m_duration) != tinyxml2::XML_SUCCESS) {
        // printf("Failed to parse duration - setting to default\n");
    }
    const char* position = root->Attribute("position");
    if (position) {
        std::sscanf(position, "%f, %f, %f", &m_emitterPosition.x, &m_emitterPosition.y, &m_emitterPosition.z);
    }

    const char* typeStr = root->Attribute("type");
    if (!typeStr) {
        // printf("Failed to parse type - setting to default\n");
    } else {
        if (std::string(typeStr) == "continuous") {
            m_mode = CONTINUOUS;
            if (root -> QueryBoolAttribute("random_birthrate", &m_bRandomBirthRate)) {
                 // printf("Failed to random birth rate - setting to default\n");
            }
            if (m_bRandomBirthRate) {
                if (root->QueryFloatAttribute("birthrate_min", &m_birthRateMin) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse birth rate min - setting to default\n");
                }
                if (root->QueryFloatAttribute("birthrate_max", &m_birthRateMax) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse birth rate max - setting to default\n");
                }
            } else {
                if (root->QueryFloatAttribute("birthrate", &m_birthRate) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse birth rate - setting to default\n");
                }
            }
        } else if (std::string(typeStr) == "burst") {
            m_mode = BURST;
            if (root -> QueryBoolAttribute("random_burstrate", &m_bRandomBurstRate)) {
                 // printf("Failed to random burst rate - setting to default\n");
            }
            if (m_bRandomBurstRate) {
                if (root->QueryFloatAttribute("burstrate_min", &m_burstRateMin) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst rate min - setting to default\n");
                }
                if (root->QueryFloatAttribute("birthrate_max", &m_burstRateMax) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst rate max - setting to default\n");
                }
            } else {
                if (root->QueryFloatAttribute("burstrate", &m_burstRate) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst rate - setting to default\n");
                }
            }
            if (root -> QueryBoolAttribute("random_burst_spawn_rate", &m_bRandomBurstSpawns)) {
                 // printf("Failed to random burst rate - setting to default\n");
            }
            if (m_bRandomBurstRate) {
                if (root->QueryFloatAttribute("burst_spawn_rate_min", &m_minBurstSpawns) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst spawn rate min - setting to default\n");
                }
                if (root->QueryFloatAttribute("burst_spawn_rate_max", &m_maxBurstSpawns) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst spawn rate max - setting to default\n");
                }
            } else {
                if (root->QueryFloatAttribute("burst_spawn_rate", &m_burstSpawns) != tinyxml2::XML_SUCCESS) {
                    // printf("Failed to parse burst spawn rate - setting to default\n");
                }
            }
            if (root -> QueryBoolAttribute("repeat_burst", &m_repeatBursts) != tinyxml2::XML_SUCCESS) {
                 // printf("Failed to repeat burst - setting to default\n");
            }
        }
        if (root -> QueryBoolAttribute("additive_blending", &m_additiveBlending) != tinyxml2::XML_SUCCESS) {
            // printf("Failed to parse additive_blending - setting to default\n");
        }
    }

    // Spawn Properties 
    tinyxml2::XMLElement *spawnProperty = root->FirstChildElement("spawn_property");
    while (spawnProperty) {
        const char* name = spawnProperty->Attribute("name");
        const char* type = spawnProperty->Attribute("type");

        if (std::string(name) == "velocity") {
            if (std::string(type) == "random") {
                const char* minStr = spawnProperty->Attribute("min");
                const char* maxStr = spawnProperty->Attribute("max");
                if (minStr && maxStr) {
                    std::sscanf(minStr, "%f, %f, %f", &m_minVelocity.x, &m_minVelocity.y, &m_minVelocity.z);
                    std::sscanf(maxStr, "%f, %f, %f", &m_maxVelocity.x, &m_maxVelocity.y, &m_maxVelocity.z);
                    m_velocityRandom = true;
                } else {
                    // printf("Failed to parse min and max, setting to default");
                }
            } else if (std::string(type) == "constant"){
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f, %f, %f", &m_velocity.x, &m_velocity.y, &m_velocity.z);
                } else {
                    // printf("Failed to parse velocity, setting to default");
                }
            }
        }
        else if (std::string(name) == "color") {
            if (std::string(type) == "random") {
                const char* minStr = spawnProperty->Attribute("min");
                const char* maxStr = spawnProperty->Attribute("max");
                if (minStr && maxStr) {
                    std::sscanf(minStr, "%f, %f, %f, %f", &m_minColorValues.r, &m_minColorValues.g, &m_minColorValues.b, &m_minColorValues.a);
                    std::sscanf(maxStr, "%f, %f, %f, %f", &m_maxColorValues.r, &m_maxColorValues.g, &m_maxColorValues.b, &m_maxColorValues.a);
                    m_colorRandom = true;
                } else {
                    // printf("Failed to parse min and max, setting to default");
                }
            } else if (std::string(type) == "constant"){
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f, %f, %f, %f", &m_color.r, &m_color.g, &m_color.b, &m_color.a);
                } else {
                    // printf("Failed to parse velocity, setting to default");
                }
            }
        }
        else if (std::string(name) == "size") {
            if (std::string(type) == "random") {
                const char* minStr = spawnProperty->Attribute("min");
                const char* maxStr = spawnProperty->Attribute("max");
                if (minStr && maxStr) {
                    std::sscanf(minStr, "%f", &m_minSize);
                    std::sscanf(maxStr, "%f", &m_maxSize);
                    m_sizeRandom = true;
                } else {
                    // printf("Failed to parse min and max, setting to default");
                }
            } else if (std::string(type) == "constant"){
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f", &m_size);
                } else {
                    // printf("Failed to parse velocity, setting to default");
                }
            }
        }
        else if (std::string(name) == "position") {
            if (std::string(type) == "box") {
                const char* minStr = spawnProperty->Attribute("min");
                const char* maxStr = spawnProperty->Attribute("max");
                if (minStr && maxStr) {
                    std::sscanf(minStr, "%f, %f, %f", &m_minPosition.x, &m_minPosition.y, &m_minPosition.z);
                    std::sscanf(maxStr, "%f, %f, %f", &m_maxPosition.x, &m_maxPosition.y, &m_maxPosition.z);
                    m_spawnShape = BOX;
                } else {
                    // printf("Failed to parse min and max, setting to default");
                }
            } else if (std::string(type) == "sphere"){
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f", &m_spawnRadius);
                } else {
                    // printf("Failed to parse velocity, setting to default");
                }
            } else if (std::string(type) == "point") {
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f, %f, %f", &m_pos.x, &m_pos.y, &m_pos.z);
                    m_spawnShape = BOX;
                } else {
                    // printf("Failed to parse value, setting to default");
                }
            }
        }

        else if (std::string(name) == "lifetime") {
            if (std::string(type) == "random") {
                const char* minStr = spawnProperty->Attribute("min");
                const char* maxStr = spawnProperty->Attribute("max");
                if (minStr && maxStr) {
                    std::sscanf(minStr, "%f", &m_minParticleLifetime);
                    std::sscanf(maxStr, "%f", &m_maxParticleLifetime);
                    m_lifetimeRandom = true;
                } else {
                    // printf("Failed to parse min and max, setting to default");
                }
            } else if (std::string(type) == "constant"){
                const char* str = spawnProperty->Attribute("value");
                if (str) {
                    std::sscanf(str, "%f", &m_particleLifetime);
                } else {
                    // printf("Failed to parse velocity, setting to default");
                }
            }
        }
        spawnProperty = spawnProperty->NextSiblingElement("spawn_property");
    }
    tinyxml2::XMLElement *affector = root->FirstChildElement("affector");
    while (affector) {
        // printf("Affector Detected - ");
        const char* affectorType = affector->Attribute("type");

        if (std::string(affectorType) == "scale") {
            // printf("Scale Affector - ");
            const char* minStr = affector->Attribute("start");
            const char* maxStr = affector->Attribute("end");
            float min = 0;
            float max = 1;
            if (minStr) {
                std::sscanf(minStr, "%f", &min);
            }
            if (maxStr) {
                std::sscanf(maxStr, "%f", &max);
            }
            ScaleAffector* scaleAffector = new ScaleAffector(min, max);
            affectors.push_back(scaleAffector);
            // printf("Added\n");
        }else if (std::string(affectorType) == "fade") {
            // printf("Fade Affector - ");
            const char* minStr = affector->Attribute("start");
            const char* maxStr = affector->Attribute("end");
            const char* typeStr = affector->Attribute("function_type");
            float min = 1;
            float max = 0;
            FunctionType p_type = LINEAR;
            if (minStr) {
                std::sscanf(minStr, "%f", &min);
            }
            if (maxStr) {
                std::sscanf(maxStr, "%f", &max);
            }
            if (typeStr && std::string(typeStr) == "parabolic") {
                p_type = PARABOLIC;
            }
            FadeAffector* fadeAffector = new FadeAffector(min, max, p_type);
            affectors.push_back(fadeAffector);
            // printf("Added\n");
        }else if (std::string(affectorType) == "velocity") {
            // printf("Velocity Affector - ");
            const char* minStr = affector->Attribute("start");
            const char* maxStr = affector->Attribute("end");
            glm::vec3 min = glm::vec3(1.0f);
            glm::vec3 max = glm::vec3(2.0f);
            if (minStr) {
                std::sscanf(minStr, "%f, %f, %f", &min.x, &min.y, &min.z);
            }
            if (maxStr) {
                std::sscanf(maxStr, "%f, %f, %f", &max.x, &max.y, &max.z);
            }
            VelocityAffector* velocityAffector = new VelocityAffector(min, max);
            affectors.push_back(velocityAffector);
            // printf("Added\n");
        }

        affector = affector->NextSiblingElement("affector");
    }
}

void Emitter::Init() {
    // printf("Emitter Initializing\n");
    for (int i = 0; i < num_particles; i++) {  
        Particle* p = CreateNewParticle(); 
        AddToFree(p);
        m_pFreeList -> num = i;
    }
    m_planeVertices = new Vertex[num_particles * 6];
    if (m_mode == BURST)
        CalcBurstTime();
    if (!m_pVertexBuffer) {
        GenerateBuffers();
    }
    // printf("Emitter Initialized\n");
}

void Emitter::GenerateBuffers() {
    // printf("Generating Vertex Buffer\n");
        m_pProgram = wolf::LoadShaders("data/particle.vsh", "data/particle.fsh");
        glGenVertexArrays(1, &m_pVertexArray);
        glBindVertexArray(m_pVertexArray);

        glGenBuffers(1, &m_pVertexBuffer);
	    glBindBuffer(GL_ARRAY_BUFFER, m_pVertexBuffer);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6 * num_particles, m_planeVertices, GL_DYNAMIC_DRAW);

        int posAttr = glGetAttribLocation(m_pProgram, "a_position");
        glVertexAttribPointer(
            posAttr,
            3, GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, x)
        );
        glEnableVertexAttribArray(posAttr);

        // Setting attribute pointer for UV coordinates
        int uvAttr = glGetAttribLocation(m_pProgram, "a_uv");
        glVertexAttribPointer(
            uvAttr,
            2, GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, u)
        );
        glEnableVertexAttribArray(uvAttr);

        // Setting attribute pointer for color
        int colorAttr = glGetAttribLocation(m_pProgram, "a_color");
        glVertexAttribPointer(
            colorAttr,
            4, GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, r)
        );
        glEnableVertexAttribArray(colorAttr);

        m_texture = wolf::TextureManager::CreateTexture("data/Circle Particle.tga");
        
        m_texture->SetFilterMode(wolf::Texture::FM_Nearest, wolf::Texture::FM_Nearest);
        m_texture->SetWrapMode(wolf::Texture::WM_Repeat);

        // printf("Vertex buffer generated\n");
}

Emitter::~Emitter() {
    wolf::TextureManager::DestroyTexture(m_texture);
    glDeleteBuffers(1, &m_pVertexBuffer);
    glDeleteVertexArrays(1, &m_pVertexArray);
}

void Emitter::UpdateSpawning(float dt) {
    switch(m_mode) {
        case CONTINUOUS:
        {
            float birth_rate = m_birthRate;
            if (m_bRandomBirthRate) {
                float range = m_birthRateMax - m_birthRateMin;
                birth_rate = m_birthRateMin + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
            }

            m_toSpawn_accumulator += birth_rate * dt;
            int num_spawns = (int) m_toSpawn_accumulator;
            m_toSpawn_accumulator -= num_spawns;

            for(int i = 0; i < num_spawns; i++) {
                SpawnParticle();
            }
        }
        break;
        case BURST:
        {
            m_burstTimer -= dt;
            if (m_burstTimer < 0) {
                float burst_spawns = 0;
                if (m_bRandomBurstSpawns) {
                    float range = m_maxBurstSpawns - m_minBurstSpawns;
                    burst_spawns = m_minBurstSpawns + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
                } else {
                    burst_spawns = m_burstSpawns;
                }
                if (m_repeatBursts) {
                    CalcBurstTime();
                }

                for (int i = 0; i < burst_spawns; i++) {
                    SpawnParticle();
                }
                // printf("Spawned %i particles. Next spawn in %f seconds\n", (int) burst_spawns, m_burstTimer);
            }
        }
        break;
    }
}

void Emitter::AddToFree(Particle* p){
    // printf("Adding to free list - ");
    p -> prev = 0;
    p -> next = m_pFreeList;
    if (m_pFreeList != nullptr) {
        // printf("Connecting previous head - ");
        m_pFreeList -> prev = p;
    }
    // printf("Particle Added to Free List \n");
    m_pFreeList = p;
}

void Emitter::AddToActive(Particle* p) {
    // printf("Adding to active list\n");
    p -> prev = 0;
    p -> next = m_pActiveListHead;
    if (m_pActiveListHead != nullptr) {
        m_pActiveListHead -> prev = p;
    } else {
        m_pActiveListTail = p;
    }
    m_pActiveListHead = p;
}

Particle* Emitter::CreateNewParticle(Particle* p_oldParticle) {
    // New particle, potentially transfer old num for display stuff
    Particle* p = new Particle();
    if (p_oldParticle) {
        int number = p_oldParticle -> num;
        p_oldParticle = new Particle();
        p = p_oldParticle;
        p -> num = number;
    }
    
    // Set the position
    glm::vec3 pos = m_pos;
    if (m_spawnShape == BOX) {
        float range = m_maxPosition.x - m_minPosition.x;
        pos.x = m_minPosition.x + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
        range = m_maxPosition.y - m_minPosition.y;
        pos.y = m_minPosition.y + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
        range = m_maxPosition.z - m_minPosition.z;
        pos.z = m_minPosition.z + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
    } else if (m_spawnShape == SPHERE) {
        pos = generateRandomPointInSphere(m_pos, m_spawnRadius);
    }
    p -> pos = pos;

    // Set the color
    glm::vec4 color = m_color;
    if (m_colorRandom) {
        float range = m_maxColorValues.r - m_minColorValues.r;
        float r = m_minColorValues.r + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        range = m_maxColorValues.g - m_minColorValues.g;
        float g = m_minColorValues.g + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        range = m_maxColorValues.b - m_minColorValues.b;
        float b = m_minColorValues.b + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        range = m_maxColorValues.a - m_minColorValues.a;
        float a = m_minColorValues.a + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        color = glm::vec4(r, g, b, a);
    }
    p->color = color;

    // Set the velocity
    glm::vec3 velocity = m_velocity;
    if (m_velocityRandom) {
        float range = m_maxVelocity.x - m_minVelocity.x;
        float xVel = m_minVelocity.x + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        range = m_maxVelocity.y - m_minVelocity.y;
        float yVel = m_minVelocity.y + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        range = m_maxVelocity.z - m_minVelocity.z;
        float zVel = m_minVelocity.z + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        velocity = glm::vec3(xVel, yVel, zVel);
    }
    p -> vel = velocity;
    p -> start_vel = velocity;

    // Set the size
    float size = m_size;
    if (m_sizeRandom) {
        float range = m_maxSize - m_minSize;
        size = m_minSize + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
    }
    p -> size = size;
    p -> start_size = size;

    // Set the lifetime 
    float pDuration = m_particleLifetime;
    if (m_lifetimeRandom) {
        float range = m_maxParticleLifetime - m_minParticleLifetime;
        pDuration = m_minParticleLifetime + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
    }
    p -> duration = pDuration;

    return p;
}

void Emitter::RemoveFromActive(Particle* p) {
    // printf("Removing from active list - ");
    if (p -> prev && p -> next) {
        p -> prev -> next = p -> next;
        p -> next -> prev = p -> prev;
        p -> prev = 0;
        p -> next = 0;
        // printf("removed middle particle\n");
    }
     else if (p -> prev) {
        m_pActiveListTail = p -> prev;
        m_pActiveListTail -> next = 0;
        p -> prev = 0;
        // printf("removed tail particle\n");
    } else if (p -> next) {
        m_pActiveListHead = p -> next;
        m_pActiveListHead -> prev = 0;
        p -> next = 0;
        // printf("removed head particle\n");
    } else {
        p -> prev = 0;
        p -> next = 0;
        m_pActiveListHead = p -> next;
        // printf("removed last active particle\n");
    }
}

Particle* Emitter::GetFreeParticle() {
    if (m_pFreeList) {
        // printf("Getting particle from free list\n");
        Particle* p = m_pFreeList;
        m_pFreeList = m_pFreeList->next;
        if (m_pFreeList) {
            m_pFreeList -> prev = 0;
        }
        p = CreateNewParticle(p);
        return p;
    } else {
        // printf("Recycling particle from active list\n");
        Particle* p = m_pActiveListTail;
        m_pActiveListTail = p -> prev;
        if (m_pActiveListTail) {
            m_pActiveListTail -> next = 0;
        }
        p = CreateNewParticle(p);
        return p;
    }
}

void Emitter::SpawnParticle() {
    if (true) {
        Particle* myPart = m_pActiveListHead;
        // printf("\nParticles: ");
        while (myPart) {
            // printf("%i, ", myPart -> num);
            myPart = myPart -> next;
        }
        // printf("\n");
    }
    // printf("Spawning Particle\n");
    Particle* p = GetFreeParticle();

    AddToActive(p);
}

void Emitter::ParticleKilled(Particle* p) {
    // printf("Killing Particle\n");
    RemoveFromActive(p);
    AddToFree(p);
}

void Emitter::KillParticles() {
    while (m_pActiveListHead) {
        Particle* currParticle = m_pActiveListHead;
        m_pActiveListHead = m_pActiveListHead -> next;
        ParticleKilled(currParticle);
    }
}

void Emitter::CalcBurstTime() {
    // printf("Calculating Burst Time: ");
    float burst_rate;
    if (m_bRandomBurstRate) {
        float range = m_burstRateMax - m_burstRateMin;
        burst_rate = m_burstRateMin + static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX / range);
    } else {
        burst_rate = m_birthRate;
    }
    // printf("%f\n", burst_rate);
    m_burstTimer = burst_rate;
}

void* LockVertexBuffer(GLuint buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (data == nullptr) {
        return nullptr;
    }

    return data;
}

void UnlockVertexBuffer(GLuint buffer) {
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Emitter::Update(float dt) {
    m_lifetime += dt;
    if (m_duration < 0 || m_lifetime < m_duration) {
        UpdateSpawning(dt);
    }
    UpdateParticles(dt);
}

void Emitter::UpdateParticles(float dt) {
    Particle* currentParticle = m_pActiveListHead;
    while (currentParticle) {
        currentParticle->pos += dt * currentParticle -> vel;
        for (Affector* affector : affectors) {
            affector -> AffectParticle(currentParticle, dt);
        }

        // This needs to happen last - kill old particles 
        if (currentParticle->duration > 0) {
            currentParticle->lifetime += dt;
            if (currentParticle-> lifetime > currentParticle->duration) {
                Particle* old_particle = currentParticle;
                currentParticle = currentParticle -> next;
                ParticleKilled(old_particle);
                continue;
            }
        }
        currentParticle = currentParticle -> next;
    }
}

void Emitter::Render(glm::mat4 p_view, glm::mat4 p_proj) {
    glm::mat4 mWorld = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    mWorld = glm::translate(mWorld, m_emitterPosition);
    glm::mat4 invertedViewMat = glm::inverse(glm::mat4(glm::mat3(p_view)));

    glUseProgram(m_pProgram);

    // Set the uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_pProgram, "projection"), 1, GL_FALSE, glm::value_ptr(p_proj));
    glUniformMatrix4fv(glGetUniformLocation(m_pProgram, "view"), 1, GL_FALSE, glm::value_ptr(p_view));
    glUniformMatrix4fv(glGetUniformLocation(m_pProgram, "world"), 1, GL_FALSE, glm::value_ptr(mWorld));

    // Binds the texture
    GLint textureSamplerLocation = glGetUniformLocation(m_pProgram, "tex1");
    m_texture->Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1i(textureSamplerLocation, 0);

    glEnable(GL_BLEND);
    if (m_additiveBlending) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);  
    
    glDepthMask(GL_FALSE);


    // Lock the vertex buffer 
    Vertex* pVerts = (Vertex*) LockVertexBuffer(m_pVertexBuffer);
    if (pVerts == nullptr) {
        return;
    }

    Particle* currentParticle = m_pActiveListHead;
    int i = 0;
    while (currentParticle) {
        glm::mat4 particleWorld = glm::translate(glm::mat4(1.0f), currentParticle->pos);
        particleWorld *= invertedViewMat;
        particleWorld = glm::rotate(particleWorld, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));

        for (int j = 0; j <= 5; j++) {
            glm::vec4 transformedVert = particleWorld * glm::vec4(
                planeVertices[j].x * currentParticle->size,
                planeVertices[j].y,
                planeVertices[j].z * currentParticle->size, 1.0f);
            pVerts[i + j].x = transformedVert.x;
            pVerts[i + j].y = transformedVert.y;
            pVerts[i + j].z = transformedVert.z;

            // Assign UV coordinates
            pVerts[i + j].u = planeVertices[j].u;
            pVerts[i + j].v = planeVertices[j].v;

            // Assign color coordinates
            pVerts[i + j].r = currentParticle->color.r;
            pVerts[i + j].g = currentParticle->color.g;
            pVerts[i + j].b = currentParticle->color.b;
            pVerts[i + j].a = currentParticle->color.a;
        }
        i += 6;
        currentParticle = currentParticle->next;
    }

    // Set remaining pVerts to be 0, 0, 0 with transparency 0
    for (i; i < num_particles * 6; i += 6) {
        glm::mat4 particleWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
        for (int j = 0; j <= 5; j++) {
            // Transform vertex position
            glm::vec4 transformedVert = particleWorld * glm::vec4(0.0f);
            pVerts[i + j].x = transformedVert.x;
            pVerts[i + j].y = transformedVert.y;
            pVerts[i + j].z = transformedVert.z;

            pVerts[i + j].a = 0.0f;
        }
    }

    UnlockVertexBuffer(m_pVertexBuffer);

    glBindVertexArray(m_pVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, num_particles * 6);

    // Unbind texture?
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}