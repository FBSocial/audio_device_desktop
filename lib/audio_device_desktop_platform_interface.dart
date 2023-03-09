import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'audio_device_desktop_method_channel.dart';

enum AudioDeviceState {
  AudioDeviceStateActive, //插入设备
  AudioDeviceStateDisabled, //禁用设备
  AudioDeviceStateNotPresent, //设备不存在
  AudioDeviceStateUnplugged, //拔出设备
  AudioDeviceStateMaskAll, //屏蔽所有设备
}

AudioDeviceState audioDeviceStateForString(String? state) {
  switch (state) {
    case 'active':
      return AudioDeviceState.AudioDeviceStateActive;
    case 'unplugged':
      return AudioDeviceState.AudioDeviceStateUnplugged;
    case 'disabled':
      return AudioDeviceState.AudioDeviceStateDisabled;
    case 'notpresent':
      return AudioDeviceState.AudioDeviceStateNotPresent;
    case 'mask_all':
      return AudioDeviceState.AudioDeviceStateMaskAll;
  }
  return AudioDeviceState.AudioDeviceStateActive;
}

abstract class AudioDeviceDesktopPlatform extends PlatformInterface {
  /// Constructs a AudioDeviceDesktopPlatform.
  AudioDeviceDesktopPlatform() : super(token: _token);

  static final Object _token = Object();

  static AudioDeviceDesktopPlatform _instance =
      MethodChannelAudioDeviceDesktop();

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
    throw UnimplementedError(
        'audioDeviceListenStart() has not been implemented.');
  }

  Future<bool?> audioDeviceListenEnd() {
    throw UnimplementedError(
        'audioDeviceListenEnd() has not been implemented.');
  }

  void setDefaultAudioDeviceChangedCallback(
      void Function(AudioDeviceState state, String kind, String deviceId)?
          callback) {
    throw UnimplementedError(
        'setDefaultAudioDeviceChangedCallback() has not been implemented.');
  }
}
