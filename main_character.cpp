
#include "main_character.h"

void MainCharacter::drawNode(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos, const glm::vec3 & lookAt) {
    Actor::drawNode(time,shaderManager,model,view,projectionView,eyePos, lookAt);
}
