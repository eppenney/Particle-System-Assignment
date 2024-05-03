#include "Effect.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

Effect::Effect() {
    
}

Effect::Effect(std::string xml_path, glm::vec3 p_position) {
    Init(xml_path, p_position);
}

void Effect::Init(std::string xml_path, glm::vec3 p_position) {
    m_path = xml_path;
    m_position = p_position;
    printf("Finding File - ");
    XMLDocument xmlDoc;

    tinyxml2::XMLError error = xmlDoc.LoadFile(xml_path.c_str());

    if (error != tinyxml2::XML_SUCCESS) {
        printf("File Not Found\n");
        return;
    }

    printf("File Found\nFinding Root - ");
    XMLElement *root = xmlDoc.RootElement();
    if (!root) {
        printf("Root not found\n");
        return;
    }
    printf("Root found\n");

    tinyxml2::XMLElement *spawnProperty = root->FirstChildElement("emitter");

    while (spawnProperty) {
        const char* name = spawnProperty->Attribute("name");
        const char* offset = spawnProperty->Attribute("offset");

        glm::vec3 p_offset = glm::vec3(0.0f);
        if (offset) {
            std::sscanf(offset, "%f, %f, %f", &p_offset.x, &p_offset.y, &p_offset.z);
        } 

        Emitter* p_emit;
        
        if (std::string(name) == "smoke") {
            p_emit = new Emitter("src/smoke.xml");
        } else if (std::string(name) == "fire") {
            p_emit = new Emitter("src/fire.xml");
        } else if (std::string(name) == "fog") {
            p_emit = new Emitter("src/fog.xml");
        }

        if (p_emit){
            p_emit -> Init();
            p_emit -> SetOffset(m_position + p_offset);
            emitters.push_back(p_emit);
        }
        
        spawnProperty = spawnProperty->NextSiblingElement("emitter");
    }

}


Effect::~Effect() {
    for (Emitter* emitter : emitters) {
        delete emitter;
    }
}

void Effect::Update(float dt) {
    if (m_state == PLAYING) {
        for (Emitter* emitter : emitters) {   
            emitter -> Update(dt);
        }
    }
}

void Effect::Render(glm::mat4 p_view, glm::mat4 p_proj) {
    if (m_state == PLAYING || m_state == PAUSED) {
        for (Emitter* emitter : emitters) {   
            emitter -> Render(p_view, p_proj);
        }
    }
}

void Effect::SetPosition(glm::vec3 p_position) { 
    m_position = p_position;
    for (Emitter* emitter : emitters) {
        emitter -> SetOffset(emitter -> GetOffset() + m_position);
    }
};

void Effect::Play() {
    if (m_state == PAUSED) {
        m_state = PLAYING;
    } else if (m_state == STOPPED){
        m_state = PLAYING;
    }
};

void Effect::Pause() {
    m_state = PAUSED;
};

void Effect::Stop() {
    m_state = STOPPED;
    for (Emitter* emitter : emitters) {
        emitter -> KillParticles();
    }
};