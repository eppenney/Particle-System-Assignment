#include "camera.h"

Camera::Camera(wolf::App* tempApp) {
    pApp = tempApp;
    direction = glm::vec3(0.0f, 0.0f, -1.0f); // Initially facing down the negative z-axis
    right = glm::vec3(1.0f, 0.0f, 0.0f);     // Initially facing right along the x-axis
    up = glm::vec3(0.0f, 1.0f, 0.0f);        // Initially facing up along the y-axis
}


Camera::~Camera(){}

void Camera::update (float dt) {

    // Orbit Camera inputs but for keys
    if (pApp->isKeyDown('W')) {
        moveForward(speed);
    }

    if (pApp -> isKeyDown('S')) {
        moveForward(-speed);
    }

    if (pApp -> isKeyDown('D')) {
        moveSideways(speed);
    }

    if (pApp -> isKeyDown('A')) {
        moveSideways(-speed);
    }

    if (pApp -> isKeyDown('E')) {
        moveVertical(speed);
    }

    if (pApp -> isKeyDown('Q')) {
        moveVertical(-speed);
    }

    if (pApp -> isKeyDown(340)) {
        maxSpeed = 100.0f;
    } else {
        maxSpeed = 50;
    }

    // Update and calculate mouse shit 
    glm::vec2 mousePos = pApp->getMousePos();
    glm::vec2 mouseDiff = mousePos - lastMousePos;
    lastMousePos = mousePos;
    rotate(mouseDiff.x * mouseSpeed, mouseDiff.y * mouseSpeed);

    updateMovement();
}

glm::mat4 Camera::getViewMatrix() {
    glm::mat4 viewMatrix = glm::lookAt(pos, pos + direction, up);
    return viewMatrix;
}

glm::mat4 Camera::getProjMatrix(int width, int height) {
    return glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 20000.0f);

}

void Camera::moveForward(float speed) {
    velocity += direction * speed;
}

void Camera::moveSideways(float speed) {
    velocity += right * speed;
}

void Camera::moveVertical(float speed) {
    velocity.y += speed;
}

void Camera::updateMovement() {
    pos += velocity;
    velocity *= 0.9;
}

void Camera::rotate(float horizontalAngle, float verticalAngle) {
    hAngle -= horizontalAngle;
    if (hAngle > glm::pi<float>() * 2 || hAngle < -glm::pi<float>() * 2) {
        hAngle = 0.0f;
    }

    vAngle = glm::clamp(vAngle - verticalAngle, -glm::pi<float>() / 2.5f, glm::pi<float>() / 2.5f);

    direction = glm::normalize(glm::vec3(
        cos(vAngle) * sin(hAngle), 
        sin(vAngle), 
        cos(vAngle) * cos(hAngle)));
    right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, direction));
}