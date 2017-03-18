#ifndef CREATESCENE_H
#define CREATESCENE_H

#include "graphics/openglrenderer.h"
#include "events/immediateevents.h"
#include "state/macrostate.h"
#include "common/stdext.h"
#include "common/pointer.h"

using PlanetShape = events::GenerateWorldEvent::PlanetShape;
using PlanetSize = events::GenerateWorldEvent::PlanetSize;

Ptr::OwningPtr<state::MacroState> createPlanetData(PlanetShape planet_shape_selector, PlanetSize planet_size_selector, int planet_seed);

void createScene(gfx::SceneNodeHandle scene_root_hdl, Ptr::ReadPtr<state::MacroState> scene_data, float &cam_view_distance);

void createMap(gfx::SceneNodeHandle scene_root_hdl, Ptr::ReadPtr<state::MacroState> scene_data);


#endif // CREATESCENE_H
