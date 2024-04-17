#pragma once

#include <glm/glm.hpp>

class Bait {
public:
    [[nodiscard]] glm::vec3 position() const { return _position; }

    glm::vec3& position() { return _position; }

private:
    glm::vec3 _position;
};
