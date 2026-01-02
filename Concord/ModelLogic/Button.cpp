#include "Button.h"
#include <Timer.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Bullet/btBulletDynamicsCommon.h>
#include <iostream>

namespace Crd
{
    namespace MdLogic
    {
        void Button::SetActive(bool val)
        {
            m_IsActive = val;
            if (m_Object)
            {
                val ? m_Object->Activate() : m_Object->Deactivate();
            }
        }

        void Button::SetMesh(Az::Mesh *mesh) { m_Mesh = mesh; }
        Az::Mesh *Button::GetMesh() { return m_Mesh; }
        void Button::SetOnDuration(float duration) { m_ON_Duration = duration; }

        void Button::Update(bool check)
        {
            if (!m_Object)
                return;

            if (IsActive() && m_CurrentTime < m_ON_Duration)
            {
                m_CurrentTime += Az::Timer::fDeltaTime;
                m_Object->Activate();
            }
            else
            {
                m_Object->Deactivate();
                SetActive(false);
                m_CurrentTime = 0;
            }

            m_Object->Update();
        }

        void Button::SetRigidBody(btRigidBody *body)
        {
            m_RigidBody = body;
            if (!m_RigidBody || !m_Mesh)
                return;

            // --- Extract scale from mesh ---
            glm::vec3 scale(
                glm::length(m_Mesh->modelMatrix[0]),
                glm::length(m_Mesh->modelMatrix[1]),
                glm::length(m_Mesh->modelMatrix[2]));

            glm::mat4 modelNoScale = m_Mesh->modelMatrix;
            modelNoScale[0] /= scale.x;
            modelNoScale[1] /= scale.y;
            modelNoScale[2] /= scale.z;

            glm::mat4 localTransform = glm::mat4(1.0f);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            m_Mesh->modelMatrix = modelNoScale * localTransform * scaleMat;

            btBoxShape *box = static_cast<btBoxShape *>(m_RigidBody->getCollisionShape());
            if (box)
                box->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(modelNoScale * localTransform));
            m_RigidBody->getMotionState()->setWorldTransform(t);
            m_RigidBody->setWorldTransform(t);
        }
    }
}
