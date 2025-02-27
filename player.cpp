#include "player.h"

Player::Player() {            //right  up   forward
    mVertices.push_back(Vertex{0.0f,  0.1f,  8.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.5f,   0.1f,  8.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    mVertices.push_back(Vertex{0.25f,   0.1f,  7.75f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f});
}
