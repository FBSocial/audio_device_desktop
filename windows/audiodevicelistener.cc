#include "audiodevicelistener.h"
#include <Functiondiscoverykeys_devpkey.h>
#include <Audioclient.h>
#include <flutter/standard_method_codec.h>

//#include "AudioInterfaceActivator.h"
using namespace std;
namespace audio_device_desktop {

#define SAFE_RELEASE(punk) \
  if ((punk) != NULL) {    \
    (punk)->Release();     \
    (punk) = NULL;         \
  }

static inline std::wstring cp_to_wide(const std::string& s, UINT codepage) {
  int in_length = (int)s.length();
  int out_length = MultiByteToWideChar(codepage, 0, s.c_str(), in_length, 0, 0);
  std::vector<wchar_t> buffer(out_length);
  if (out_length)
    MultiByteToWideChar(codepage, 0, s.c_str(), in_length, &buffer[0],
                        out_length);
  std::wstring result(buffer.begin(), buffer.end());
  return result;
}

static inline std::string wide_to_cp(const std::wstring& s, UINT codepage) {
  int in_length = (int)s.length();
  int out_length =
      WideCharToMultiByte(codepage, 0, s.c_str(), in_length, 0, 0, 0, 0);
  std::vector<char> buffer(out_length);
  if (out_length)
    WideCharToMultiByte(codepage, 0, s.c_str(), in_length, &buffer[0],
                        out_length, 0, 0);
  std::string result(buffer.begin(), buffer.end());
  return result;
}

std::string wstr_to_utf8(LPWSTR wValue) {
  int len = WideCharToMultiByte(CP_UTF8, 0, wValue, -1, 0, 0, 0, 0);
  std::string value(len, 0);
  WideCharToMultiByte(CP_UTF8, 0, wValue, -1, &value[0], len, 0, 0);
  if (value.length() > 0 && value[value.length() - 1] == 0) {
    value = value.substr(0, value.length() - 1);
  }
  return value;
}

static inline std::string utf8_to_cp(const std::string& s, UINT codepage) {
  if (codepage == CP_UTF8)
    return s;
  std::wstring wide = cp_to_wide(s, CP_UTF8);
  return wide_to_cp(wide, codepage);
}

static inline std::string wide_to_ansi(const std::wstring& s) {
  return wide_to_cp(s, CP_ACP);
}

static inline std::string utf8_to_ansi(const std::string& s) {
  return utf8_to_cp(s, CP_ACP);
}

void printHResult(const char* errorMessage, HRESULT hr) {
  DWORD error = GetLastError();
  printf("%s (hresult %lu 0x%x lasterror %d 0x%x)\n", errorMessage, hr, hr,
         error, error);
}

AudioDeviceListener::AudioDeviceListener(std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel)
    : _methodChannel(std::move(channel)),
    _cRef(1),
    _pEnumerator(NULL),
    _hasRegister(false) {}

AudioDeviceListener::~AudioDeviceListener(){SAFE_RELEASE(_pEnumerator)}

bool AudioDeviceListener::onRegister() {
  HRESULT hr = S_OK;
  // 初始化COM
  hr = ::CoInitialize(NULL);
  if (FAILED(hr)) {
    cout << "CoInitialize Error" << endl;
  }
  // 创建接口
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                        __uuidof(IMMDeviceEnumerator), (void**)&_pEnumerator);

  if (hr == S_OK) {
    //cout << "Create Instance Ok" << endl;
  } else {
    cout << "AudioDeviceListener onRegister Create Instance Failed" << endl;
  }

  // 注册事件
  hr = _pEnumerator->RegisterEndpointNotificationCallback(
      (IMMNotificationClient*)this);
  if (hr == S_OK) {
    _hasRegister = true;
    cout << "AudioDeviceListener Register Ok" << endl;
  } else {
    cout << "AudioDeviceListener Register Failed" << endl;
  }
  CoUninitialize();
  return hr == S_OK;
}

bool AudioDeviceListener::onUnregister() {
  HRESULT hr = S_OK;
  if (_pEnumerator && _hasRegister) {
    hr = _pEnumerator->UnregisterEndpointNotificationCallback(
        (IMMNotificationClient*)this);
    if (hr == S_OK) {
      _hasRegister = false;
      cout << "AudioDeviceListener Unregister Ok" << endl;
      SAFE_RELEASE(_pEnumerator)
    } else {
      cout << "AudioDeviceListener Unregister Failed" << endl;
    }
  }
  return hr == S_OK;
}

ULONG STDMETHODCALLTYPE AudioDeviceListener::AddRef() {
  return InterlockedIncrement(&_cRef);
}

ULONG STDMETHODCALLTYPE AudioDeviceListener::Release() {
  ULONG ulRef = InterlockedDecrement(&_cRef);
  if (0 == ulRef) {
    _methodChannel = nullptr;
    delete this;
  }
  return ulRef;
}

HRESULT __stdcall AudioDeviceListener::QueryInterface(REFIID riid,
                                                 VOID** ppvInterface) {
  if (IID_IUnknown == riid) {
    AddRef();
    *ppvInterface = (IUnknown*)this;
  } else if (__uuidof(IMMNotificationClient) == riid) {
    AddRef();
    *ppvInterface = (IMMNotificationClient*)this;
  } else {
    *ppvInterface = NULL;
    return E_NOINTERFACE;
  }
  return S_OK;
}

HRESULT __stdcall AudioDeviceListener::OnDefaultDeviceChanged(
    EDataFlow flow,
    ERole role,
    LPCWSTR pwstrDeviceId) {
  const char* pszFlow = "?????";
  const char* pszRole = "?????";

  //_PrintDeviceName(pwstrDeviceId);

  switch (flow) {
    case eRender:
      pszFlow = "output";
      break;
    case eCapture:
      pszFlow = "input";
      break;
  }
  switch (role) {
    case eConsole:
      pszRole = "eConsole";
      break;
    case eMultimedia:
      pszRole = "eMultimedia";
      break;
    case eCommunications: {
      pszRole = "eCommunications";
    } break;
  }

  std::string strDeviceName;
  std::string strDeviceId;
  if (pwstrDeviceId) {
    strDeviceId = wide_to_ansi(pwstrDeviceId);
    if (role == eConsole) {
      if (flow == eRender) {
//        base_->activeAudioOutputDevice(pwstrDeviceId);
      }
      // plug in the earphone
      if (flow == eCapture) {
//        base_->activeAudioInputDevice(pwstrDeviceId);
      }
    }
    strDeviceName = _GetDeviceNameById(pwstrDeviceId);
  }
  if (strDeviceId.empty()) {
      return S_OK;
  }
  auto iter = _currentStateMap.find(strDeviceId);
  if (role == eCommunications && flow != eCapture) {
      if (iter != _currentStateMap.end() && iter->second.state == DEVICE_STATE_UNPLUGGED) {
        // only DeviceState unplugg notify
        std::cout << " ==== device state unplugged and default device changed ====" << std::endl;
      }
//    base_->setAudioInput(0);
  }
  //cout << "  \n----------------------\nflow = " << pszFlow << endl
  //     << "  role = " << pszRole << endl 
  //     << "  id   = " << strDeviceId << endl
  //     << "  name = " << strDeviceName.c_str() <<
  //    "\n-----------------------------\n" << endl;
  //output device use eMultimedia type, input device use eCommunications
  if ((ERole::eMultimedia == role && eRender == flow) || ERole::eCommunications == role && eCapture == flow) {
    std::string strState = "None";
    if (iter != _currentStateMap.end()) {
      iter->second.flow = flow;
      strState = _DeviceStateFromDW(iter->second.state);
    }

    cout << "  ====>New default device: flow = " << pszFlow
         << ", role = " << pszRole << ", id = " << strDeviceId
         << " name = " << strDeviceName.c_str() << " state: " << strState << endl;
    OnDefaultAudioDeviceChanged(strState, pszRole, pszFlow, strDeviceName, strDeviceId);
  }
  return S_OK;
}

HRESULT __stdcall AudioDeviceListener::OnDeviceAdded(LPCWSTR pwstrDeviceId) {
  //_PrintDeviceName(pwstrDeviceId);

  printf("  ---->Added device\n");
  return S_OK;
}

HRESULT __stdcall AudioDeviceListener::OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
  //_PrintDeviceName(pwstrDeviceId);

  printf("  ---->Removed device\n");
  return S_OK;
}

HRESULT __stdcall AudioDeviceListener::OnDeviceStateChanged(LPCWSTR pwstrDeviceId,
                                                       DWORD dwNewState) {
  //_PrintDeviceName(pwstrDeviceId);
  std::string strState = _DeviceStateFromDW(dwNewState);
  std::string strDeviceId = wide_to_ansi(pwstrDeviceId);
  _currentStateMap[strDeviceId].state = dwNewState;
  printf("  ******>New device state is DEVICE_STATE_%s (0x%8.8x) id = %s\n",
      strState.c_str(), dwNewState, strDeviceId.c_str());

  return S_OK;
}

HRESULT __stdcall AudioDeviceListener::OnPropertyValueChanged(
    LPCWSTR pwstrDeviceId,
    const PROPERTYKEY key) {
  // printf("  -->Changed device property "
  //    "{%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%d\n",
  //    key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
  //    key.fmtid.Data4[0], key.fmtid.Data4[1],
  //    key.fmtid.Data4[2], key.fmtid.Data4[3],
  //    key.fmtid.Data4[4], key.fmtid.Data4[5],
  //    key.fmtid.Data4[6], key.fmtid.Data4[7],
  //    key.pid);
  return S_OK;
}

std::string AudioDeviceListener::getSystemDefaultAudioDeviceId(EDataFlow dataFlow) {
  HRESULT hr;
  LPWSTR id;
  IMMDevice* device;

  ERole role = eConsole;
  std::string strDeviceId;
  hr = _pEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &device);
  if (FAILED(hr)) {
    printHResult("GetDefaultAudioEndpoint failed", hr);
    return strDeviceId;
  }

  // printf("  DefaultDevice: ");

  hr = device->GetId(&id);
  if (hr) {
    printHResult("GetDefaultAudioEndpoint failed", hr);
  } else {
    strDeviceId = id ? wide_to_ansi(id) : "";
  }

  IPropertyStore* pProps = NULL;

  hr = device->OpenPropertyStore(STGM_READ, &pProps);
  if (FAILED(hr)) {
    printHResult("GetDefaultAudioEndpoint failed", hr);
    return strDeviceId;
  }

  PROPVARIANT varName;

  PropVariantInit(&varName);

  hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
  if (FAILED(hr)) {
    printHResult("GetDefaultAudioEndpoint failed", hr);
  } else {
    printf("GetDefaultAudioEndpoint: %s\n",
           wide_to_cp(varName.pwszVal, CP_ACP).c_str());
    PropVariantClear(&varName);
    pProps->Release();
  }

  device->Release();

  return strDeviceId;
}

void AudioDeviceListener::resetDeviceState(const std::string& kind, const std::string& deviceId) {
    auto iter = _currentStateMap.find(deviceId);
    if (iter != _currentStateMap.end()) {
        _currentStateMap.erase(iter);
    }
}

HRESULT AudioDeviceListener::_PrintDeviceName(LPCWSTR pwstrId) {
  HRESULT hr = S_OK;
  IMMDevice* pDevice = NULL;
  IPropertyStore* pProps = NULL;
  PROPVARIANT varString;

  CoInitialize(NULL);
  PropVariantInit(&varString);

  if (_pEnumerator == NULL) {
    // Get enumerator for audio endpoint devices.
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                          CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
                          (void**)&_pEnumerator);
  }
  if (hr == S_OK) {
  }
  if (hr == S_OK) {
    hr = _pEnumerator->GetDevice(pwstrId, &pDevice);
  }
  if (hr == S_OK) {
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
  }
  if (hr == S_OK) {
    // Get the endpoint device's friendly-name property.
    PropVariantInit(&varString);
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
  }
  cout << "----------------------\nDevice name: "
       << ((hr == S_OK) ? wide_to_cp(varString.pwszVal, CP_ACP) : "null device")
       << endl
        << "  Endpoint ID string: "
       << ((pwstrId != NULL) ? wide_to_cp(pwstrId, CP_ACP) : "null ID")
      << "---------------------------\n" << endl;
  if (hr == S_OK) {
    PropVariantClear(&varString);
  }

  SAFE_RELEASE(pProps)
  SAFE_RELEASE(pDevice)
  CoUninitialize();
  return hr;
}

std::string AudioDeviceListener::_GetDeviceNameById(LPCWSTR pwstrId) {
  HRESULT hr = S_OK;
  IMMDevice* pDevice = NULL;
  IPropertyStore* pProps = NULL;
  PROPVARIANT varString;
  std::string strDeviceName;
  CoInitialize(NULL);
  PropVariantInit(&varString);

  if (_pEnumerator == NULL) {
    // Get enumerator for audio endpoint devices.
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
                          CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
                          (void**)&_pEnumerator);
  }
  if (hr == S_OK) {
  }
  if (hr == S_OK) {
    hr = _pEnumerator->GetDevice(pwstrId, &pDevice);
  }
  if (hr == S_OK) {
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
  }
  if (hr == S_OK) {
    // Get the endpoint device's friendly-name property.
    PropVariantInit(&varString);
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
  }
  strDeviceName = wstr_to_utf8(varString.pwszVal);
  if (hr == S_OK) {
    PropVariantClear(&varString);
  }

  SAFE_RELEASE(pProps)
  SAFE_RELEASE(pDevice)
  CoUninitialize();

  return strDeviceName;
}

std::string AudioDeviceListener::_DeviceStateFromDW(DWORD state) {
  std::string strState = "None";
  switch (state) {
    case DEVICE_STATE_ACTIVE: {
        strState = "active";
    } break;
    case DEVICE_STATE_DISABLED:
        strState = "disabled";
      break;
    case DEVICE_STATE_NOTPRESENT:
        strState = "notpresent";
      break;
    case DEVICE_STATE_UNPLUGGED: {
        strState = "unplugged";
    } break;
    case DEVICE_STATEMASK_ALL:
        strState = "mask_all";
      // default:
      //  break;
  }
  return strState;
}

void AudioDeviceListener::OnDefaultAudioDeviceChanged(const std::string& state,
  const std::string& role, const std::string& flow,
  const std::string& deviceName, const std::string& deviceId)
{
  auto&& event = flutter::EncodableValue(flutter::EncodableMap {
    {flutter::EncodableValue("event"), flutter::EncodableValue("onDefaultAudioDeviceChanged")},
    { flutter::EncodableValue("state"), flutter::EncodableValue(state) },
    { flutter::EncodableValue("role"), flutter::EncodableValue(role) },
    { flutter::EncodableValue("kind"), flutter::EncodableValue(flow) },
    { flutter::EncodableValue("deviceName"), flutter::EncodableValue(deviceName) },
    { flutter::EncodableValue("deviceId"), flutter::EncodableValue(deviceId) },
  });
  if (_methodChannel) {
    _methodChannel->InvokeMethod("audioDeviceEvent", std::make_unique<flutter::EncodableValue>(event));
  }
}

}  // namespace audio_device_desktop