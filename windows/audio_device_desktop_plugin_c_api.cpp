#include "include/audio_device_desktop/audio_device_desktop_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "audio_device_desktop_plugin.h"

void AudioDeviceDesktopPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  audio_device_desktop::AudioDeviceDesktopPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
