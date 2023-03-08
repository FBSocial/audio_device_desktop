import Cocoa
import FlutterMacOS
import CoreAudio
import Foundation

public class AudioDeviceDesktopPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "audio_device_desktop", binaryMessenger: registrar.messenger)
    let instance = AudioDeviceDesktopPlugin(channel)
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  private let channel: FlutterMethodChannel

  public init(_ channel: FlutterMethodChannel) {
    self.channel = channel
    super.init()
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getPlatformVersion":
      result("macOS " + ProcessInfo.processInfo.operatingSystemVersionString)
    case "audioDeviceListenStart":
      let res = listen()
      result(res)
      case "audioDeviceListenEnd":
      let res = unlisten()
      result(res)
    default:
      result(FlutterMethodNotImplemented)
    }
  }

  private var listening = false

  func listen() -> Bool {
      if (listening) {
          unlisten()
      }

      let systemObjectID = AudioObjectID(kAudioObjectSystemObject)
      let clientData = Unmanaged.passUnretained(self).toOpaque()
      var address = AudioObjectPropertyAddress(
          mSelector: kAudioObjectPropertySelectorWildcard,
          mScope: kAudioObjectPropertyScopeWildcard,
          mElement: kAudioObjectPropertyElementWildcard
      )
      if noErr == AudioObjectAddPropertyListener(systemObjectID, &address, onAudioPropertyChanged, clientData) {
        listening = true
        return true
      } else {
        return false
      }
  }

  func unlisten() -> Bool {
      if !listening {
          return false
      }

      let systemObjectID = AudioObjectID(kAudioObjectSystemObject)
      let clientData = Unmanaged.passUnretained(self).toOpaque()
      var address = AudioObjectPropertyAddress(
          mSelector: kAudioObjectPropertySelectorWildcard,
          mScope: kAudioObjectPropertyScopeWildcard,
          mElement: kAudioObjectPropertyElementWildcard
      )

      if noErr == AudioObjectRemovePropertyListener(systemObjectID, &address, onAudioPropertyChanged, clientData) {
        listening = false
        return true
      } else {
        return false
      }
  }

  fileprivate func onDefaultDeviceChanged(isInput: Bool) {
     channel.invokeMethod("audioDeviceEvent", arguments: ["event": "onDefaultAudioDeviceChanged",
      "state": "",
      "role": "",
      "kind": isInput ? "input": "output",
      "deviceName": "",
      "deviceId": ""])

     if isInput {
      print("--------- onDefaultInputDeviceChanged ---------");
     } else {
      print("--------- onDefaultOutputDeviceChanged ---------");
     }
  }
}

fileprivate func onAudioPropertyChanged(
  objectID: UInt32,
  numInAddresses: UInt32,
  inAddresses: UnsafePointer<AudioObjectPropertyAddress>,
  clientData: Optional<UnsafeMutableRawPointer>
) -> Int32 {
  let address = inAddresses.pointee
  let _self = Unmanaged<AudioDeviceDesktopPlugin>.fromOpaque(clientData!).takeUnretainedValue()

  switch address.mSelector {
      case kAudioHardwarePropertyDefaultInputDevice://default input device changed
          _self.onDefaultDeviceChanged(isInput: true)

      case kAudioHardwarePropertyDefaultOutputDevice://default output device changed
          _self.onDefaultDeviceChanged(isInput: false)

      case kAudioHardwarePropertyDefaultSystemOutputDevice://default system output device changed
          break // nop?
      default:
          break // nop
  }

  return kAudioHardwareNoError
}