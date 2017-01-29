#ifndef RENDERFLAGS_H
#define RENDERFLAGS_H

namespace gfx {

struct RenderFlags {
    enum Flags {
        Default      = 0b00000000,
        Wireframe    = 0b00000001,
        Hidden       = 0b00000010,
    };

public:
    RenderFlags() : mFlags(Default) {}
private:
    RenderFlags(Flags flags) : mFlags(flags) {}
public:
    inline static RenderFlags combine(RenderFlags rflags1, RenderFlags rflags2)
    {
        return RenderFlags(static_cast<Flags>(rflags1.mFlags | rflags2.mFlags));
    }

    inline bool checkFlag(Flags flag) const { return ( (mFlags & flag) == flag); } // PARENTHESIS NEEDED

    inline void setFlag(Flags flag) { mFlags |= flag; }
    inline void clearFlag(Flags flag) { mFlags &= ~flag; }
    inline void toggleFlag(Flags flag) { mFlags ^= flag; }

private:
    unsigned char mFlags;
};



}

#endif // RENDERFLAGS_H
