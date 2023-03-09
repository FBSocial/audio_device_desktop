import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'audio_device_desktop_platform_interface.dart';

/// An implementation of [AudioDeviceDesktopPlatform] that uses method channels.
class MethodChannelAudioDeviceDesktop extends AudioDeviceDesktopPlatform {
  /// The method channel used to interact with the native platform.
  //@visibleForTesting
  final _methodChannel = const MethodChannel('audio_device_desktop');
  static void Function(AudioDeviceState state, String kind, String deviceId)?
      onDefaultAudioDeviceChanged;

  MethodChannelAudioDeviceDesktop() {
    _methodChannel.setMethodCallHandler((MethodCall call) async {
      if (onDefaultAudioDeviceChanged == null) return;
      switch (call.method) {
        case "audioDeviceEvent":
          {
            final arg = call.arguments as Map;
            if (arg.containsKey('event') &&
                arg['event'] == 'onDefaultAudioDeviceChanged') {
              final String stateStr = arg['state'];
              final String kind = arg['kind'];
              final String deviceId = arg['deviceId'];
              final AudioDeviceState state =
                  audioDeviceStateForString(stateStr);
              onDefaultAudioDeviceChanged?.call(state, kind, deviceId);
            }
          }
          break;
        default:
          break;
      }
    });
  }

  @override
  Future<String?> getPlatformVersion() async {
    final version =
        await _methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<bool?> audioDeviceListenStart() async {
    return _methodChannel.invokeMethod<bool>('audioDeviceListenStart');
  }

  @override
  Future<bool?> audioDeviceListenEnd() async {
    return _methodChannel.invokeMethod<bool>('audioDeviceListenEnd');
  }

  void setDefaultAudioDeviceChangedCallback(
      void Function(AudioDeviceState state, String kind, String deviceId)?
          callback) {
    onDefaultAudioDeviceChanged = callback;
  }
}
