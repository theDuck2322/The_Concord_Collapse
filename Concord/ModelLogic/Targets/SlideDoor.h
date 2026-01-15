#include <mdControlled.h>
#include <Door.h>

namespace Crd
{
    namespace MdLogic
    {
        class SlideDoor : public Door
        {
        public:
            void Update(bool firstCheck = true) override;

        protected:
            glm::mat4 ComputeTransform(float offset) const override;
        };
    }
}