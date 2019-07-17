#include <events/create_plane_trigger_event.h>
#include "drawable_processor.h"
#include "events/events.h"
#include "actor.h"
#include "scene_container.h"
#include "work_queue.h"
#include "camera.h"
#include "timer/timer.h"
#include "animation/animation_manager.h"
#include "game.h"
#include "triggers/plane_trigger.h"

DrawableProcessor::DrawableProcessor(engine * engine) {
    gameEngine = engine;
}

DrawableProcessor::~DrawableProcessor() {
    gameEngine = NULL;
}

void DrawableProcessor::processDrawableEvent(DrawableEvent *event) {
    switch (event->getEventType()) {
        case CHARACTER_EVENT_ID:
            handleCharacter((CharacterEvent*)event);
            break;
        case CREATE_ANIMATION_EVENT_ID:
            handleCamera((CameraAnimationEvent *) event);
            break;
        case PRELOAD_CHARACTER_EVENT_ID:
            handlePreloadCharacter((PreloadCharacterEvent *) event);
            break;
        case CREATE_FOLLOWING_EVENT_ID:
            handleFollowingEvent((CameraFollowingEvent *) event);
            break;
        case REGISTER_TIMER_EVENT_ID:
            handleTimerCreation((RegisterTimerEvent *) event);
            break;
        case UNLOCK_EVENT_ID:
            handleUnlockEvent((UnlockEvent *) event);
            break;
        case UNLOCK_GRAVITY_EVENT_ID:
            handleUnlockGravityEvent((UnlockGravityEvent *) event);
            break;
        case TEXT_ANIMATION_EVENT_ID:
            handleTextAnimationEvent((TextAnimationEvent *) event);
            break;
        case DYNAMIC_CHARACTER_EVENT_ID:
            handleDynamicCharacter((DynamicCharacterEvent *) event);
            break;
        case RESTART_EVENT_ID:
            handleRestartEvent((RestartEvent *) event);
            break;
        case CREATE_PLANE_TRIGGER_EVENT_ID:
            handleCreateTriggerEvent((CreatePlaneTriggerEvent *) event);
            break;
        case WON_EVENT_ID:
            handleWonEvent((GameWonEvent *) event);
            break;
    }
}

void DrawableProcessor::handleCharacter(const CharacterEvent * evnt) {
    createCharacterFromEvent(evnt);
}

void DrawableProcessor::handleDynamicCharacter(const DynamicCharacterEvent * evnt) {
    createCharacterFromEvent(evnt);
}

void DrawableProcessor::handlePreloadCharacter(const PreloadCharacterEvent * evnt) {
    createCharacterFromEvent(evnt);
}

void DrawableProcessor::createCharacterFromEvent(const CharacterEvent * evnt) {
    PActorDescriptor descriptor = Actor::createDescriptor();

    if ( evnt->hasPosition()) {
        descriptor->position = evnt->getPosition();
    }

    if (evnt->hasScale()) {
        descriptor->scale = evnt->getScale();
    }

    if (evnt->getTexture().size()) {
        descriptor->texture = string(evnt->getTexture());
    }

    if (evnt->getVert().size()) {
        descriptor->vertex = string(evnt->getVert());
    }

    if (evnt->getFrag().size()) {
        descriptor->fragment = string(evnt->getFrag());
    }

    descriptor->uniforms = evnt->getUniforms();

    Actor * newCharacter = new Actor(evnt->getDescriptor(),gameEngine->drawableState->modelManager,gameEngine->drawableState->shaderManager,gameEngine->drawableState->textureManager, &descriptor);

    if (evnt->hasVelocity()) {
        newCharacter->accelerate(evnt->getVelocity());
    }

    newCharacter->setCharacterEventId(evnt->getEventId());

    evnt->addToScene(newCharacter,gameEngine);
    evnt->getPromise()->resolve(evnt->handleActorCreated(newCharacter));

    // If we are just pre-loading we are going to destroy the character,
    // so we are not going to resolve the promise with the character itself
    if (evnt->getEventType() != PRELOAD_CHARACTER_EVENT_ID) {
        gameEngine->workQueue->addEvent(new CharacterCreatedEvent(newCharacter));
    }
}

void DrawableProcessor::handleCamera(const CameraAnimationEvent *event) {
    AnimationState * animation = event->createAnimationState();
    gameEngine->drawableState->camera->addState(animation);
}

void DrawableProcessor::handleTimerCreation(const RegisterTimerEvent *event) {
    gameEngine->drawableState->timerManager->addTimer(event->getEventId(), event->createTimer());
}

void DrawableProcessor::handleRestartEvent(const RestartEvent* event) {
    if (gameEngine->game_state < STATE_GAME_SHOULD_RESTART) {
        gameEngine->game->restart();
    }
}

void DrawableProcessor::handleCreateTriggerEvent(const CreatePlaneTriggerEvent *event) {
    gameEngine->drawableState->screenContainer->addNode(new PlaneTrigger(gameEngine->workQueue,event->getNormal(),event->getPlaneD(),event->getCallback(),event->getEventId()));
}

void DrawableProcessor::handleUnlockEvent(const UnlockEvent* event) {
    if (gameEngine->game_state < STATE_UNLOCKED) {
        gameEngine->game_state = STATE_UNLOCKED;
        ALOGE("UNLOCK EVENT! GAME STATE! %d", gameEngine->game_state);
        gameEngine->drawableState->screenContainer->addSceneBounds();
    }
}

void DrawableProcessor::handleUnlockGravityEvent(const UnlockGravityEvent *) {
    if (gameEngine->game_state < STATE_GRAVITY_ENABLED) {
        gameEngine->game_state = STATE_GRAVITY_ENABLED;
    }
}

void DrawableProcessor::handleWonEvent(const GameWonEvent *) {
    if (gameEngine->game_state < STATE_GAME_WON) {
        gameEngine->game_state = STATE_GAME_WON;
    }
}

void DrawableProcessor::handleFollowingEvent(const CameraFollowingEvent *event) {
    gameEngine->drawableState->camera->addState(event->createAnimationState(gameEngine->drawableState->character));
}

void DrawableProcessor::handleTextAnimationEvent(TextAnimationEvent *event) {
    Animation * animation = event->takeAnimation();

    if (!animation->getParent()) {
        gameEngine->drawableState->animationManager->addAnimation(event->getEventId(), animation);
    }

    event->getPromise()->resolve(animation);
}

