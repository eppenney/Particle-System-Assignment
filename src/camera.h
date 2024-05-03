#pragma once
#include "../wolf/wolf.h"

class Camera {
    public:
        Camera(wolf::App* tempApp);
        ~Camera();

        void update(float dt);
        glm::mat4 getViewMatrix();
        glm::mat4 getProjMatrix(int width, int height);
        glm::vec3 GetPosition() { return pos; };

    private:
        // Movement Methods
        void moveForward(float speed);
        void moveSideways(float speed);
        void moveVertical(float speed);
        void rotate(float horizontalAngle, float verticalAngle);
        void updateMovement();

        // Positioning Data
        float hAngle;
        float vAngle;
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 direction;
        glm::vec3 right;
        glm::vec3 up;

        //Misc
        float speed = 0.1f;
        float maxSpeed = 1.0f;
        glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        float mouseSpeed = 0.01f;
        wolf::App* pApp = nullptr;
        glm::vec2 lastMousePos = glm::vec2(0.0f, 0.0f);
};