#include "scene_container.h"
#include "parallel_plane.h"
#include <algorithm>
#include "actor.h"
#include "events/died_event.h"
#include "work_queue.h"
#include "game.h"

void SceneContainer::addDynamic(INode *node) {
    //dynamicContainer.addNode(node);
}

void SceneContainer::addSceneBounds() {
    //BoundingRect rect = getBoundingRect();
    //ParallelPlane * plane = new ParallelPlane(NEGATIVE_Y,rect.bottom.y);
    //addNode(plane);
}

BoundingRect SceneContainer::getBoundingRect() const {
    return currentBounds.expand(0.5f);
}

BoundingSphere SceneContainer::getBoundingSphere() const {
    return createBoundingSphere(currentBounds);
}

void SceneContainer::addNode(INode *node) {
    INode::addNode(node);
    if (node->getNodeType() == NODE_TYPE_ACTOR) {
        IGravitable *currentSceneElement = (IGravitable *) node;

        BoundingRect rect = currentSceneElement->getBoundingRect();

        if (currentBounds.bottom.x > rect.bottom.x) {
            currentBounds.bottom.x = rect.bottom.x;
        }

        if (currentBounds.top.x < rect.top.x) {
            currentBounds.top.x = rect.top.x;
        }

        if (currentBounds.bottom.y > rect.bottom.y) {
            currentBounds.bottom.y = rect.bottom.y;
        }

        if (currentBounds.top.y < rect.top.y) {
            currentBounds.top.y = rect.top.y;
        }

        if (currentBounds.bottom.z > rect.bottom.z) {
            currentBounds.bottom.z = rect.bottom.z;
        }

        if (currentBounds.top.z < rect.top.z) {
            currentBounds.top.z = rect.top.z;
        }
    }
}

void SceneContainer::processNodeEvents(meow_time_t time,engine * gameEngine) {
    if(IS_GRAVITY_ENABLED(gameEngine->game_state) && !IS_GAME_END(gameEngine->game_state) ) {
        ICollidable * gravitable = checkCollision(gameEngine->drawableState->character);
        if ( gravitable ) {
            if (gravitable->collidedWith(gameEngine->drawableState->character)) {
                gameEngine->game_state = STATE_GAME_COLLISION;
                gameEngine->workQueue->addEvent(new DiedEvent(Collision));
            }
            //gameEngine->game->restart();
        } else if (!gameEngine->drawableState->character->isWithinBounds(getBoundingRect())) {
            gameEngine->game_state = STATE_GAME_COLLISION;
            gameEngine->workQueue->addEvent(new DiedEvent(OutOfBounds));
            //gameEngine->game->restart();
        }
    } else {
        Container::processNodeEvents(time,gameEngine);
    }
}

void SceneContainer::processNode(meow_time_t time, engine *gameEngine) {
    INode::processNode(time, gameEngine);
}
