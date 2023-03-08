#ifndef FLUTTER_PLUGIN_AUDIO_DEVICE_DESKTOP_PLUGIN_H_
#define FLUTTER_PLUGIN_AUDIO_DEVICE_DESKTOP_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>
//#include "sharewindowborder.h"
#include "audiodevicelistener.h"

namespace audio_device_desktop {

class AudioDeviceDesktopPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  AudioDeviceDesktopPlugin(flutter::PluginRegistrarWindows* registrar,
    std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel);

  virtual ~AudioDeviceDesktopPlugin();

  // Disallow copy and assign.
  AudioDeviceDesktopPlugin(const AudioDeviceDesktopPlugin&) = delete;
  AudioDeviceDesktopPlugin& operator=(const AudioDeviceDesktopPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  //std::unique_ptr<ShareWindowBorder> shareWindowBorder_;
  std::unique_ptr<AudioDeviceListener> audioDeviceListener_;
};

}  // namespace audio_device_desktop

#endif  // FLUTTER_PLUGIN_AUDIO_DEVICE_DESKTOP_PLUGIN_H_
