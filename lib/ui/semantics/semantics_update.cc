// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/semantics/semantics_update_builder.h"

#include <memory>

#include "flutter/lib/ui/painting/matrix.h"
#include "lib/tonic/converter/dart_converter.h"
#include "lib/tonic/dart_args.h"
#include "lib/tonic/dart_binding_macros.h"
#include "lib/tonic/dart_library_natives.h"

namespace blink {

IMPLEMENT_WRAPPERTYPEINFO(ui, SemanticsUpdate);

#define FOR_EACH_BINDING(V) V(SemanticsUpdate, dispose)

DART_BIND_ALL(SemanticsUpdate, FOR_EACH_BINDING)

fxl::RefPtr<SemanticsUpdate> SemanticsUpdate::create(
    SemanticsNodeUpdates nodes,
    CustomAccessibilityActionUpdates actions) {
  return fxl::MakeRefCounted<SemanticsUpdate>(std::move(nodes), std::move(actions));
}

SemanticsUpdate::SemanticsUpdate(SemanticsNodeUpdates nodes,
                                 CustomAccessibilityActionUpdates actions)
    : nodes_(std::move(nodes)), actions_(std::move(actions)) {}

SemanticsUpdate::~SemanticsUpdate() = default;

SemanticsNodeUpdates SemanticsUpdate::takeNodes() {
  return std::move(nodes_);
}

CustomAccessibilityActionUpdates SemanticsUpdate::takeActions() {
  return std::move(actions_);
}

void SemanticsUpdate::dispose() {
  ClearDartWrapper();
}

}  // namespace blink
