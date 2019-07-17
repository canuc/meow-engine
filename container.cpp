#include "container.h"
#include <algorithm>
#include "actor.h"
#include "events/died_event.h"
#include "work_queue.h"
#include "actor.h"
#include "triggers/trigger.h"

Container::~Container() {
    vector<INode *>::iterator child = nodes();

    while (child != end()) {
        int nodeType = (*child)->getNodeType();
        ALOGI("Calling Destroy on: %d with address: %p",nodeType, (*child));
        switch(nodeType) {
            case NODE_TYPE_ACTOR: {
                INode * currentChild = *child;
                delete currentChild;
                break;
            }
            case NODE_TYPE_CONTAINER: {
                break;
            }
        }
        child++;
    }

    m_nodeTree.clear();
}

glm::vec3 Container::getForces(const IGravitable * gravitable) const {
     glm::vec3 overallForces;

     for(std::vector<INode *>::const_iterator it = m_nodeTree.begin(); it != m_nodeTree.end(); it++ ) {
         glm::vec3 gravitableForce;

         switch((*it)->getNodeType()) {
             case NODE_TYPE_ACTOR: {
                 const IGravitable *currentSceneForce = (const IGravitable *) *it;
                 gravitableForce = currentSceneForce->calculateGravitationalForce(
                         gravitable);
                 break;
             }
             case NODE_TYPE_CONTAINER: {
                 const Container *container = (const Container *) *it;

                 gravitableForce = container->getForces(gravitable);
                 break;
             }
         }
        overallForces += gravitableForce;
     }

     return overallForces;
}

void Container::drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
              const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
              const glm::vec3 &lookAt) {
    std::sort(m_nodeTree.begin(),m_nodeTree.end());

    return;
}

ICollidable * Container::checkCollision(const IGravitable * position) {
    IGravitable * currentSceneElement;
    Container * container;

    for(std::vector<INode *>::iterator it = m_nodeTree.begin(); it != m_nodeTree.end(); it++ ) {
        switch((*it)->getNodeType()) {
            case NODE_TYPE_ACTOR: {
                // this is bad
                currentSceneElement = (IGravitable *) *it;

                if (currentSceneElement->collides(position)) {
                    return currentSceneElement;
                }

                break;
            }
            case NODE_TYPE_CONTAINER: {
                container = (Container *) *it;

                ICollidable *containerCollision = container->checkCollision(position);

                if (containerCollision) {
                    return containerCollision;
                }

                break;
            }
            case NODE_TYPE_TRIGGER: {
                Trigger * trigger = (Trigger *) *it;

                if (trigger->collides(position)) {
                    return trigger;
                }
                break;
            }
        }

    }

    return NULL;
}

