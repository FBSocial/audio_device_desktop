import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:audio_device_desktop/audio_device_desktop_method_channel.dart';

void main() {
  MethodChannelAudioDeviceDesktop platform = MethodChannelAudioDeviceDesktop();
  const MethodChannel channel = MethodChannel('audio_device_desktop');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
