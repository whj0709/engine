// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/darwin/ios/platform_view_ios.h"

#import <QuartzCore/CAEAGLLayer.h>

#include <utility>

#include "flutter/common/task_runners.h"
#include "flutter/fml/trace_event.h"
#include "flutter/shell/common/io_manager.h"
#include "flutter/shell/platform/darwin/ios/framework/Source/vsync_waiter_ios.h"
#include "flutter/shell/platform/darwin/ios/ios_external_texture_gl.h"
#include "lib/fxl/synchronization/waitable_event.h"

namespace shell {

PlatformViewIOS::PlatformViewIOS(PlatformView::Delegate& delegate,
                                 blink::TaskRunners task_runners,
                                 FlutterViewController* owner_controller,
                                 FlutterView* owner_view)
    : HeadlessPlatformViewIOS(delegate, std::move(task_runners)),
      owner_controller_(owner_controller),
      owner_view_(owner_view),
      ios_surface_(owner_view_.createSurface) {
  FXL_DCHECK(ios_surface_ != nullptr);
  FXL_DCHECK(owner_controller_ != nullptr);
  FXL_DCHECK(owner_view_ != nullptr);
}

PlatformViewIOS::~PlatformViewIOS() = default;

FlutterViewController* PlatformViewIOS::GetOwnerViewController() const {
  return owner_controller_;
}

void PlatformViewIOS::RegisterExternalTexture(int64_t texture_id,
                                              NSObject<FlutterTexture>* texture) {
  RegisterTexture(std::make_shared<IOSExternalTextureGL>(texture_id, texture));
}

// |shell::PlatformView|
std::unique_ptr<Surface> PlatformViewIOS::CreateRenderingSurface() {
  return ios_surface_->CreateGPUSurface();
}

// |shell::PlatformView|
sk_sp<GrContext> PlatformViewIOS::CreateResourceContext() const {
  if (!ios_surface_->ResourceContextMakeCurrent()) {
    FXL_DLOG(INFO) << "Could not make resource context current on IO thread. Async texture uploads "
                      "will be disabled.";
    return nullptr;
  }

  return IOManager::CreateCompatibleResourceLoadingContext(GrBackend::kOpenGL_GrBackend);
}

// |shell::PlatformView|
void PlatformViewIOS::SetSemanticsEnabled(bool enabled) {
  if (enabled && !accessibility_bridge_) {
    accessibility_bridge_ = std::make_unique<AccessibilityBridge>(owner_view_, this);
  } else if (!enabled && accessibility_bridge_) {
    accessibility_bridge_.reset();
  }
  PlatformView::SetSemanticsEnabled(enabled);
}

// |shell:PlatformView|
void PlatformViewIOS::SetAssistiveTechnologyEnabled(bool enabled) {
  if (enabled && !accessibility_bridge_) {
    accessibility_bridge_ = std::make_unique<AccessibilityBridge>(owner_view_, this);
  }
  // Note: since the accessibility bridge is needed for both semantics and
  // assistive technologies, but you cannot have the latter without the
  // former, we only destroy the bridge in SetSemanticsEnabled and not here.
  PlatformView::SetAssistiveTechnologyEnabled(enabled);
}

// |shell::PlatformView|
void PlatformViewIOS::UpdateSemantics(blink::SemanticsNodeUpdates update,
                                      blink::CustomAccessibilityActionUpdates actions) {
  if (accessibility_bridge_) {
    accessibility_bridge_->UpdateSemantics(std::move(update), std::move(actions));
  }
}

// |shell::PlatformView|
std::unique_ptr<VsyncWaiter> PlatformViewIOS::CreateVSyncWaiter() {
  return std::make_unique<VsyncWaiterIOS>(task_runners_);
}

fml::scoped_nsprotocol<FlutterTextInputPlugin*> PlatformViewIOS::GetTextInputPlugin() const {
  return text_input_plugin_;
}

void PlatformViewIOS::SetTextInputPlugin(fml::scoped_nsprotocol<FlutterTextInputPlugin*> plugin) {
  text_input_plugin_ = plugin;
}

}  // namespace shell
