#pragma once
#include <Button.h>

namespace Crd
{
    namespace MdLogic
    {
        class Switch : public Button
        {
        public:
            void Update(bool check = true) override;
        };
    }
}
