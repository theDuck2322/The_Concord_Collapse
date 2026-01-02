#include <Switch.h>

namespace Crd
{
    namespace MdLogic
    {
        void Switch::Update(bool check)
        {
            if (!m_Object)
                return;

            m_IsActive ? m_Object->Activate() : m_Object->Deactivate();
            if (m_RigidBody && m_Mesh)
                m_Object->Update();
        }
    }
}
