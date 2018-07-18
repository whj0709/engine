// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_LAYERS_LAYER_H_
#define FLUTTER_FLOW_LAYERS_LAYER_H_

#include <memory>
#include <vector>

#include "flutter/flow/instrumentation.h"
#include "flutter/flow/raster_cache.h"
#include "flutter/flow/texture.h"
#include "flutter/glue/trace_event.h"
#include "lib/fxl/build_config.h"
#include "lib/fxl/logging.h"
#include "lib/fxl/macros.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkRect.h"

#if defined(OS_FUCHSIA)

#include "flutter/flow/scene_update_context.h"  //nogncheck
#include "lib/ui/scenic/cpp/resources.h"     //nogncheck
#include "lib/ui/scenic/cpp/session.h"       //nogncheck

#endif  // defined(OS_FUCHSIA)

namespace flow {

// This should be an exact copy of the Clip enum in painting.dart.
//
// We call it Clip in public Dart API to provide our developers the shortest
// name and the best experience. We call it ClipMode in C++ because we want to
// avoid name conflicts and refactoring C++ names without a nice IDE function
// is tedious.
enum ClipMode {
  none,
  hardEdge,
  antiAlias,
  antiAliasWithSaveLayer
};

class ContainerLayer;

// Represents a single composited layer. Created on the UI thread but then
// subquently used on the Rasterizer thread.
class Layer {
 public:
  Layer();
  virtual ~Layer();

  struct PrerollContext {
    RasterCache* raster_cache;
    GrContext* gr_context;
    SkColorSpace* dst_color_space;
    SkRect child_paint_bounds;
  };

  virtual void Preroll(PrerollContext* context, const SkMatrix& matrix);

  struct PaintContext {
    SkCanvas& canvas;
    const Stopwatch& frame_time;
    const Stopwatch& engine_time;
    TextureRegistry& texture_registry;
    const bool checkerboard_offscreen_layers;
  };

  // Calls SkCanvas::saveLayer and restores the layer upon destruction. Also
  // draws a checkerboard over the layer if that is enabled in the PaintContext.
  class AutoSaveLayer {
   public:
    AutoSaveLayer(const PaintContext& paint_context,
                  const SkRect& bounds,
                  const SkPaint* paint);

    AutoSaveLayer(const PaintContext& paint_context,
                  const SkCanvas::SaveLayerRec& layer_rec);

    ~AutoSaveLayer();

   private:
    const PaintContext& paint_context_;
    const SkRect bounds_;
  };

  virtual void Paint(PaintContext& context) const = 0;

#if defined(OS_FUCHSIA)
  // Updates the system composited scene.
  virtual void UpdateScene(SceneUpdateContext& context);
#endif

  ContainerLayer* parent() const { return parent_; }

  void set_parent(ContainerLayer* parent) { parent_ = parent; }

  bool needs_system_composite() const { return needs_system_composite_; }
  void set_needs_system_composite(bool value) {
    needs_system_composite_ = value;
  }

  const SkRect& paint_bounds() const { return paint_bounds_; }

  // This must be set by the time Preroll() returns otherwise the layer will
  // be assumed to have empty paint bounds (paints no content).
  void set_paint_bounds(const SkRect& paint_bounds) {
    paint_bounds_ = paint_bounds;
  }

  bool needs_painting() const { return !paint_bounds_.isEmpty(); }

 private:
  ContainerLayer* parent_;
  bool needs_system_composite_;
  SkRect paint_bounds_;

  FXL_DISALLOW_COPY_AND_ASSIGN(Layer);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_LAYER_H_
