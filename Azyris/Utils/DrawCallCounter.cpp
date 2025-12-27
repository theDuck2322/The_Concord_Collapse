#include <DrawCallCounter.h>

namespace Az
{
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
    {
        glDrawElements(mode, count, type, indices);
        I_DrawCallsCounter += 1;
    }
    void DrawArrays(GLenum mode, GLint first, GLsizei count)
    {
        glDrawArrays(mode, first, count);
        I_DrawCallsCounter += 1;
    }
    int GetDrawCallsCounter()
    {
        return I_DrawCallsCounter;
    }
    void ResetDrawCallCounter()
    {
        I_DrawCallsCounter = 0;
    }
}