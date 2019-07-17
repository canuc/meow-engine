#include "user_control.h"
#include "actor.h"

void IUserControl::handleControl(Actor *character,const IInput *input) {
    glm::vec2 vec = input->inputVec();
    character->accelerate(glm::vec3(vec,0));
}

void IUserControl::pollInput(meow_time_t time) {

}