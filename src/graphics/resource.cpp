#include "resource.h"

namespace gfx {

namespace Resource {

Manager<texture_tag> &TextureManager()
{
   static Manager<texture_tag> texture_manager;
   return texture_manager;
}

}

}
