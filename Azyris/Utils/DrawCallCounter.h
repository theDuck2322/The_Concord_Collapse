#pragma once
#include <glad/glad.h>

namespace Az
{
    inline int I_DrawCallsCounter = 0;

    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

    void DrawArrays(GLenum mode, GLint first, GLsizei count);

    int GetDrawCallsCounter();

    void ResetDrawCallCounter();

}