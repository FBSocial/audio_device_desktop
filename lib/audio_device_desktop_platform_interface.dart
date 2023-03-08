import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'audio_device_desktop_method_channel.dart';

abstract class AudioDeviceDesktopPlatform extends PlatformInterface {
  /// Constructs a AudioDeviceDesktopPlatform.
  AudioDeviceDesktopPlatform() : super(token: _token);

  static final Object _token = Object();

  static AudioDeviceDesktopPlatform _instance = MethodChannelAudioDeviceDesktop();

  /// The default instance of [AudioDeviceDesktopPlatform] to use.
  ///
  /// Defaults to [MethodChannelAudioDeviceDesktop].
  static AudioDeviceDesktopPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [AudioDeviceDesktopPlatform] when
  /// they register themselves.
  static set instance(AudioDeviceDesktopPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<bool?> audioDeviceListenStart() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
  Future<bool?> audioDeviceListenEnd() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
