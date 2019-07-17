//
// Created by julian on 2/7/16.
//

#include "events/dynamic_character_event.h"
#include "scene_container.h"
#include "actor.h"

void DynamicCharacterEvent::addToScene(Actor *actor, engine *gameEngine) const {
    gameEngine->drawableState->screenContainer->addDynamic(actor);
}