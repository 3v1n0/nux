// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Jason Smith <jason.smith@canonical.com>
 */

#ifndef COVERFLOWVIEW_H
#define COVERFLOWVIEW_H

#include "NuxCore/Math/Vector4.h"
#include "CoverflowModel.h"
#include "ClientArea.h"

#if defined(NUX_OS_WINDOWS)
  #define PKGDATADIR "../../data/"
#endif

namespace nux
{
  class Coverflow: public nux::ClientArea
  {
    NUX_DECLARE_OBJECT_TYPE(Coverflow, ClientArea);
  public:
    Coverflow();
    ~Coverflow();

    Property<unsigned int> animation_length;

    Property<float> camera_motion_drift_angle;
    
    Property<bool> camera_motion_drift_enabled;

    Property<float> edge_fade;

    Property<int> flat_icons;
    
    Property<float> folding_angle;

    Property<float> folding_depth;

    Property<float> folding_rate;

    Property<float> fov;

    Property<float> kinetic_scroll_rate;

    Property<float> mouse_drag_rate;

    Property<float> pinching;

    Property<bool> pop_out_selected;

    Property<float> reflection_fadeout_distance;
    
    Property<float> reflection_strength;

    Property<float> space_between_icons;

    Property<CoverflowModel::Ptr> model;

    Property<bool> show_labels;

    Property<bool> show_drop_shadow;
    
    Property<bool> show_reflection;

    Property<bool> true_perspective;

    Property<float> y_offset;

    Property<float> reflection_size;

    bool AcceptKeyNavFocus();

    void SetCameraDistance(float distance);

    float ViewportWidthAtDepth(float depth);
    float ViewportHeightAtDepth(float depth);
  protected:
    virtual bool InspectKeyEvent(unsigned int eventType, unsigned int keysym, const char* character);
    virtual void ClientDraw(nux::GraphicsEngine& graphics_engine, nux::DrawAreaContext &ctx, bool force_draw);

  private:
    struct Impl;
    Impl* pimpl;
  };
}

#endif // COVERFLOWVIEW_H

