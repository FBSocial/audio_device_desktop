import 'package:flutter_test/flutter_test.dart';
import 'package:audio_device_desktop/audio_device_desktop.dart';
import 'package:audio_device_desktop/audio_device_desktop_platform_interface.dart';
import 'package:audio_device_desktop/audio_device_desktop_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockAudioDeviceDesktopPlatform
    with MockPlatformInterfaceMixin
    implements AudioDeviceDesktopPlatform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final AudioDeviceDesktopPlatform initialPlatform =
      AudioDeviceDesktopPlatform.instance;

  test('$MethodChannelAudioDeviceDesktop is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelAudioDeviceDesktop>());
  });

  test('getPlatformVersion', () async {
    AudioDeviceDesktop audioDeviceDesktopPlugin = AudioDeviceDesktop();
    MockAudioDeviceDesktopPlatform fakePlatform =
        MockAudioDeviceDesktopPlatform();
    AudioDeviceDesktopPlatform.instance = fakePlatform;

    expect(await audioDeviceDesktopPlugin.getPlatformVersion(), '42');
  });
}
