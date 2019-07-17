/*
 * Copyright 2019 Julian Haldenby
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef INODE_H
#define INODE_H

#include <vector>
using namespace std;
#include "meow.h"
#include <glm/vec3.hpp>
#include "drawable.h"

#define NODE_TYPE_ABSTRACT 0

class INode: public Drawable
{
    public:
        INode():parent(NULL) {};
        virtual ~INode() {};

        virtual void addNode(INode * node) {
            if (node->setParent(this) ) {
                 m_nodeTree.push_back(node);
            }
        }

        virtual void removeNode(INode * node) {
            for(std::vector<INode *>::iterator it = m_nodeTree.begin(); it != m_nodeTree.end(); it++ ) {
                if (*it == node) {
                    (*it)->unsetParent(this);
                    m_nodeTree.erase(it);

                    break;
                }
            }

            return;
        }

        void draw(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                  const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                  const glm::vec3 &lookAt) {
            glm::mat4 currentModel = transformModelViewPreRender(model);
            glm::mat4 currentProjectionView = transformProjectionPreRender(projectionView);

            drawNode(time, shaderManager, currentModel, view, currentProjectionView, eyePos,
                     lookAt);

            glm::mat4 transformedModelView = transformModelView(currentModel);
            glm::mat4 transformedProjectionView = transformProjection(currentProjectionView);

            for(std::vector<INode *>::iterator it = m_nodeTree.begin(); it != m_nodeTree.end(); it++ ) {
                (*it)->draw(time, shaderManager, transformedModelView, view,
                            transformedProjectionView, eyePos, lookAt);
            }

            drawAfter(time, shaderManager, transformedModelView, view, transformedProjectionView, eyePos);
        }

        virtual void drawAfter(meow_time_t time, ShaderManager * shaderManager,const glm::mat4 & model, const glm::mat4 & view, const glm::mat4 & projectionView, const glm::vec3 & eyePos) {
            // NO-OP
        }

        bool setParent(INode * node) {
            bool addedParent;
            if ( parent == NULL ) {
                parent = node;
                addedParent = true;
            } else {
                ALOGE("ERROR: Cannot set parent the node already has a parent.\n");
                addedParent = false;
            }

            return addedParent;
        }

        void unsetParent(INode * node) {
            if ( parent != node && parent != NULL ) {
                ALOGE("Warning: Parent is not equal to the parent that is bieng removed.\n");
            }

            parent = NULL;
        }

        virtual void processNodeEvents(meow_time_t time, struct engine* gameEngine) {
            processNode(time,gameEngine);

            for(std::vector<INode *>::iterator it = m_nodeTree.begin(); it != m_nodeTree.end(); it++ ) {
                (*it)->processNodeEvents(time, gameEngine);
            }
        }

        vector<INode *>::iterator nodes() {
            return m_nodeTree.begin();
        }

        vector<INode *>::iterator end() {
            return m_nodeTree.end();
        }

        vector<INode *>::iterator erase(vector<INode *>::iterator erase) {
            return m_nodeTree.erase(erase);
        }

        virtual int getNodeType() const {
            return NODE_TYPE_ABSTRACT;
        }

    protected:
        vector<INode *> m_nodeTree;
        INode * parent;

        virtual void drawNode(meow_time_t time, ShaderManager *shaderManager, const glm::mat4 &model,
                      const glm::mat4 &view, const glm::mat4 &projectionView, const glm::vec3 &eyePos,
                      const glm::vec3 &lookAt)=0;

        virtual glm::mat4 transformModelView(const glm::mat4 & modelView) const {
            return glm::mat4(modelView);
        }

        virtual glm::mat4 transformProjection(const glm::mat4 & projectionView) const {
            return glm::mat4(projectionView);
        }

        virtual glm::mat4 transformModelViewPreRender(const glm::mat4 & modelView) const {
            return glm::mat4(modelView);
        }

        virtual glm::mat4 transformProjectionPreRender(const glm::mat4 & projection) const {
            return glm::mat4(projection);
        }

        virtual void processNode(meow_time_t time, struct engine * gameEngine) {
            UNUSED(gameEngine);
            UNUSED(time);
        }

};

#endif // INODE_H
