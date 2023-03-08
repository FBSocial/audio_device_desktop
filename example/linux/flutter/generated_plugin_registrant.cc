//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <audio_device_desktop/audio_device_desktop_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) audio_device_desktop_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "AudioDeviceDesktopPlugin");
  audio_device_desktop_plugin_register_with_registrar(audio_device_desktop_registrar);
}
