// Copyright 2017 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FLOW_LAYERS_DEFAULT_LAYER_BUILDER_H_
#define FLUTTER_FLOW_LAYERS_DEFAULT_LAYER_BUILDER_H_

#include <stack>

#include "flutter/flow/layers/container_layer.h"
#include "flutter/flow/layers/layer_builder.h"
#include "garnet/public/lib/fxl/macros.h"

namespace flow {

class DefaultLayerBuilder final : public LayerBuilder {
 public:
  DefaultLayerBuilder();

  // |flow::LayerBuilder|
  ~DefaultLayerBuilder() override;

  // |flow::LayerBuilder|
  void PushTransform(const SkMatrix& matrix) override;

  // |flow::LayerBuilder|
  void PushClipRect(const SkRect& rect, ClipMode clip_mode = ClipMode::antiAlias) override;

  // |flow::LayerBuilder|
  void PushClipRoundedRect(const SkRRect& rect, ClipMode clip_mode = ClipMode::antiAlias) override;

  // |flow::LayerBuilder|
  void PushClipPath(const SkPath& path, ClipMode clip_mode = ClipMode::antiAlias) override;

  // |flow::LayerBuilder|
  void PushOpacity(int alpha) override;

  // |flow::LayerBuilder|
  void PushColorFilter(SkColor color, SkBlendMode blend_mode) override;

  // |flow::LayerBuilder|
  void PushBackdropFilter(sk_sp<SkImageFilter> filter) override;

  // |flow::LayerBuilder|
  void PushShaderMask(sk_sp<SkShader> shader,
                      const SkRect& rect,
                      SkBlendMode blend_mode) override;

  // |flow::LayerBuilder|
  void PushPhysicalShape(const SkPath& path,
                         double elevation,
                         SkColor color,
                         SkColor shadow_color,
                         SkScalar device_pixel_ratio,
                         ClipMode clip_mode) override;

  // |flow::LayerBuilder|
  void PushPerformanceOverlay(uint64_t enabled_options,
                              const SkRect& rect) override;

  // |flow::LayerBuilder|
  void PushPicture(const SkPoint& offset,
                   SkiaGPUObject<SkPicture> picture,
                   bool picture_is_complex,
                   bool picture_will_change) override;

  // |flow::LayerBuilder|
  void PushTexture(const SkPoint& offset,
                   const SkSize& size,
                   int64_t texture_id) override;

#if defined(OS_FUCHSIA)
  // |flow::LayerBuilder|
  void PushChildScene(const SkPoint& offset,
                      const SkSize& size,
                      fxl::RefPtr<flow::ExportNodeHolder> export_token_holder,
                      bool hit_testable) override;
#endif  // defined(OS_FUCHSIA)

  // |flow::LayerBuilder|
  void Pop() override;

  // |flow::LayerBuilder|
  std::unique_ptr<flow::Layer> TakeLayer() override;

 private:
  std::unique_ptr<flow::ContainerLayer> root_layer_;
  flow::ContainerLayer* current_layer_ = nullptr;

  std::stack<SkRect> cull_rects_;

  void PushLayer(std::unique_ptr<flow::ContainerLayer> layer,
                 const SkRect& cullRect);

  FXL_DISALLOW_COPY_AND_ASSIGN(DefaultLayerBuilder);
};

}  // namespace flow

#endif  // FLUTTER_FLOW_LAYERS_DEFAULT_LAYER_BUILDER_H_
