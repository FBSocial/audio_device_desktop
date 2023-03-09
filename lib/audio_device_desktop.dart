import 'audio_device_desktop_platform_interface.dart';

class AudioDeviceDesktop {
  Future<String?> getPlatformVersion() {
    return AudioDeviceDesktopPlatform.instance.getPlatformVersion();
  }

  Future<bool?> audioDeviceListenStart() {
    return AudioDeviceDesktopPlatform.instance.audioDeviceListenStart();
  }

  Future<bool?> audioDeviceListenEnd() {
    return AudioDeviceDesktopPlatform.instance.audioDeviceListenEnd();
  }

  void setDefaultAudioDeviceChangedCallback(
      void Function(AudioDeviceState state, String kind, String deviceId)?
          callback) {
    AudioDeviceDesktopPlatform.instance
        .setDefaultAudioDeviceChangedCallback(callback);
  }
}
