
import 'audio_device_desktop_platform_interface.dart';

class AudioDeviceDesktop {
  Future<String?> getPlatformVersion() {
    return AudioDeviceDesktopPlatform.instance.getPlatformVersion();
  }
}
