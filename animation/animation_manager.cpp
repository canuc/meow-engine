#include "animation/animation_manager.h"
#include "animation/animation.h"
#include "vector"

AnimationManager::AnimationManager(){

}

AnimationManager::~AnimationManager() {
    ALOGE("Freeing animation manager!");
}

const Animation * AnimationManager::getAnimation(int32_t animationId) {
    return getElement(animationId);
}

void AnimationManager::animate(meow_time_t currentTime) {
    ConstElementMap * allAllocatedElements = elementMap();
    Manager<Animation>::ElementConstIterator iter = allAllocatedElements->begin();
    std::vector<int32_t> animationsToDelete;

    while (iter != allAllocatedElements->end()) {
        bool shouldDelete = iter->second->drawAnimation(currentTime);
        if (shouldDelete) {
            animationsToDelete.push_back(iter->first);
        }
        iter++;
    }

    vector<int32_t>::iterator animationToDelete = animationsToDelete.begin();

    while (animationToDelete != animationsToDelete.end()) {
        deleteElement(*animationToDelete);
        animationToDelete++;
    }

    return;
}

void AnimationManager::addAnimation(int32_t id, Animation *animation) {
    addElement(id,animation);
}
