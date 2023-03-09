import 'audio_device_desktop_platform_interface.dart';

class AudioDeviceDesktop {
  Future<String?> getPlatformVersion() {
    return AudioDeviceDesktopPlatform.instance.getPlatformVersion();
  }

  Future<bool?> audioDeviceListenStart() {
    print('---- start listen audio device ----');
    return AudioDeviceDesktopPlatform.instance.audioDeviceListenStart();
  }

  Future<bool?> audioDeviceListenEnd() {
    print('---- end listen audio device ----');
    return AudioDeviceDesktopPlatform.instance.audioDeviceListenEnd();
  }

  void setDefaultAudioDeviceChangedCallback(
      void Function(AudioDeviceState state, String kind, String deviceId)?
          callback) {
    AudioDeviceDesktopPlatform.instance
        .setDefaultAudioDeviceChangedCallback(callback);
  }
}
