#ifndef _CMMNOTIFICATIONCLIENT_H_
#define _CMMNOTIFICATIONCLIENT_H_

#include <mmdeviceapi.h>
#include <iostream>
#include <map>
#include <vector>
#include <windows.h>
#include <flutter/method_channel.h>

namespace audio_device_desktop {

struct DeviceStateInfo
{
    EDataFlow flow = EDataFlow::eRender;
    DWORD state = 0x00000000;
};

class AudioDeviceListener : public IMMNotificationClient 
{
public:
  AudioDeviceListener(std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel);
  virtual ~AudioDeviceListener();
  //开始注册监听
  bool onRegister();
  //注销设备监听
  bool onUnregister();

  ULONG STDMETHODCALLTYPE AddRef();
  ULONG STDMETHODCALLTYPE Release();
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

  // Callback methods for device-event notifications.
  HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow,
                                                   ERole role,
                                                   LPCWSTR pwstrDeviceId);
  HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
  HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
  HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId,
                                                 DWORD dwNewState);
  HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId,
                                                   const PROPERTYKEY key);
  
  std::string getSystemDefaultAudioDeviceId(EDataFlow dataFlow);
  void resetDeviceState(const std::string& kind, const std::string& deviceId);

  void OnDefaultAudioDeviceChanged(const std::string& state,
    const std::string& role,
    const std::string& flow,
    const std::string& deviceName,
    const std::string& deviceId);

 private:
  // Private function to print device-friendly name
  HRESULT _PrintDeviceName(LPCWSTR pwstrId);
  std::string _GetDeviceNameById(LPCWSTR pwstrId);
  std::string _DeviceStateFromDW(DWORD state);

 private:
  LONG _cRef;
  IMMDeviceEnumerator* _pEnumerator;
  bool _hasRegister;
  std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> _methodChannel;
  std::map<std::string, DeviceStateInfo> _currentStateMap;
};
}  // namespace audio_device_desktop
#endif  //_DEVICELISTENER_H_