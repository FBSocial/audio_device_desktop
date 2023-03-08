#include "audio_device_desktop_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace audio_device_desktop {

// static
void AudioDeviceDesktopPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
      registrar->messenger(), "audio_device_desktop",
      &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<AudioDeviceDesktopPlugin>(registrar, std::move(channel));
  registrar->AddPlugin(std::move(plugin));
}

AudioDeviceDesktopPlugin::AudioDeviceDesktopPlugin(flutter::PluginRegistrarWindows* registrar,
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel) {
  channel->SetMethodCallHandler(
    [this](const auto& call, auto result) {
      this->HandleMethodCall(call, std::move(result));
  });
  audioDeviceListener_ = std::make_unique<AudioDeviceListener>(std::move(channel));
  //if (!shareWindowBorder_) {
  //  shareWindowBorder_ = std::make_unique<ShareWindowBorder>();
  //  shareWindowBorder_->create();
  //}
}

AudioDeviceDesktopPlugin::~AudioDeviceDesktopPlugin() {
  audioDeviceListener_->onUnregister();
  //PostMessage(shareWindowBorder_->getHwnd(), WM_STOP_WINDOW_BORDER, 0, 0);
}

void AudioDeviceDesktopPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    }
    else if (IsWindows8OrGreater()) {
      version_stream << "8";
    }
    else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    audioDeviceListener_->onRegister();
    result->Success(flutter::EncodableValue(version_stream.str()));
  }
  //else if (method_call.method_name().compare("startShowBorder") == 0) {
  //  HWND hWnd = (HWND)0x001701FC;
  //  if (hWnd) {
  //    PostMessage(shareWindowBorder_->getHwnd(), WM_START_WINDOW_BORDER, (WPARAM)hWnd, 0);
  //  }
  //  result->Success();
  //}
  else {
    result->NotImplemented();
  }
}

}  // namespace audio_device_desktop
