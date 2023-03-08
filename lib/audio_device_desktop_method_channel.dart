import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'audio_device_desktop_platform_interface.dart';

/// An implementation of [AudioDeviceDesktopPlatform] that uses method channels.
class MethodChannelAudioDeviceDesktop extends AudioDeviceDesktopPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('audio_device_desktop');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<bool?> audioDeviceListenStart() async {
    return methodChannel.invokeMethod<bool>('audioDeviceListenStart');
  }
  
  @override
  Future<bool?> audioDeviceListenEnd() async {
    return methodChannel.invokeMethod<bool>('audioDeviceListenEnd');
  }
}
