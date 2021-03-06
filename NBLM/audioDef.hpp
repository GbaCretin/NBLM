#ifndef AUDIODEF_HPP
#define AUDIODEF_HPP

namespace audioDef
{
    enum Note : int
    {
        C,
        CS,
        D,
        DS,
        E,
        F,
        FS,
        G,
        GS,
        A,
        AS,
        B,
        NOTE_COUNT
    };

    enum Panning : int
    {
        NONE,
        LEFT,
        RIGHT,
        CENTER
    };
}
#endif // AUDIODEF_HPP
