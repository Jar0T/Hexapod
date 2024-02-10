# Hexapod controller
* [General info](general-info)
* [Requirements](requirements)
* [Usage](usage)
* [Functionalities](functionalities)
* [Additional resources](additional-resources)

## General info
This is mobile application for controlling hexapod robot

## Requirements
Running project requires:
1. Visual Studio with .NET Multi-platform App UI development including Xamarin package
2. Android device with at least Android 4.3 supporting BLE

iOS devices are currently not supported

## Usage
1. Run Visual studio solution
2. Enable USB Debugging on your Android device
3. Connect device to USB port on your computer
4. Run project

## Functionalities
Application is built based on MVVM architectural pattern. It uses BLE service for dealing with BLE communication.

### BLE Service
It is responsible for communicating with BLE devices. Its functionalities are defined by public interface IBleService. The functionalities are:
1. Finding BLE devices

```
Task<IEnumerable<IDevice>> GetDevicesAsync();
```

Method searches for available Bluetooth devices.

It returns list of found BLE devices.
2. Connecting to selected BLE device

```
Task ConnectToDeviceAsync(IDevice device);
```

Method connects to a selected BLE device.

Method argument is device - IDevice that should be connected to.
3. Disconnecting from selected device

```
Task DisconnectFromDeviceAsync(IDevice device);
```

Method disconnects from a selected BLE device.

Method argument is device - IDevice that should be disconnected from.
4. Finding device's services

```
Task<IEnumerable<IService>> GetServicesAsync(IDevice device);
```

Method searches for services of selected device.

Method argument is device - IDevice that services belong to. 

It returns list of available services.
5. Finding service's characteristics

```
Task<IEnumerable<ICharacteristic>> GetCharacteristicsAsync(IService service);
```

Method searches for characteristics of a service.

Method argument is service - IService that characteristics belong to.

It returns list of available characteristics.
6. Sending data to selected characteristic

```
Task WriteDataAsync(ICharacteristic characteristic, byte[] data);
```

Method sends data to selected BLE characteristic.

Method arguments are characteristic - ICharacteristic that data is sent to and data - array of bytes sent to characteristic.
7. Reading data from selected characteristic

```
Task<byte[]> ReadDataAsync(ICharacteristic characteristic);
```

Method reads data from selected characteristic.

Method argument is characteristic - ICharacteristic, that data is read from.

Method returns array of bytes read from characteristic.
8. Enabling/disabling BLE notifications

```
Task RegisterForNotificationsAsync(ICharacteristic characteristic, bool update, Action<ICharacteristic> onUpdate);
```

Method enalbes/disables notifications from characteristic.

Method arguments are characteristic - ICharacteristic to enable/disable notifications, update - bool flag, true to enable notifications, false to disable notifications, onUpdate - delegate called when data is received from characteristic, it takes ICharacteristic that sent data as argument.

### BLE devices page
It consists of BleDevicesPage View and BleDevicesViewModel ViewModel.

BleDevicesViewModel stores available BLE devices in ObservalbeCollection of IDevice called BleDevices. It has BleScanCommand and DeviceSelectedCommand.

BleScanCommand is a command that takes no arguments and is responsible for searching for available BLE devices. When executed it calls OnBleScan delegate that checks if required permissions have been granted. If not then device asks for premissions. If permissions have been granted then BleService scans for available devices. Discovered devices are stored in BleDevices collection.

DeviceSelectedCommand is a command that takes IDevie as an argument and is responsible for connecting to a device and switching to RcControlPage. When executed it calls OnDeviceSelected delegate that first checks if device is already connected. If device is not connected then it tries to connect. If device is connected then page is switched to RcControlPage. If device is still disconnected method returns.


The view consists of "Scan" button that starts process of searching for available Bluetooth devices. Found devices are displayed on list below "Scan" button. Other displayed information are device Id and signal strength. Tapping list item results in connecting to a device and switching to RcControlPage.

### RC control page
It consists of RcControlPage View and RcControlViewModel ViewModel.

RcControlViewModel contains IDevice, list of IServices and list of ICharacteristics. It contains following properties: Vector2 Direction, float Rotation, float Height and bool NotificationsEnabled. It has commands MovementJoystickMovedCommand, RotationJoystickMovedCommand and ResetRobotCommand and implements methods for writing steering data to BLE device and receifing data updates from BLE devices.

RcControlViewModel constructor takes IDevice device as argument. If device isn't null then method LoadServicesAndCharacteristics searches for IServices available to device and ICharacteristics available to services. Available services and caharcteristics are stored in _services and _characteristics Lists accordingly. If device contains service with GUID 00000001-61d1-11ee-8c99-0242ac120002 then constructor starts async Task WriteMovementData that sneds data periodically. Otherwise device is disconnected and page is poped to BleDevicesPage.

WriteMovementData async task checks every 100 ms if Vector2 Direction is not zero and if Rotation is not zero. If Direction is not zero then WriteDirection method is called. If Rotation is not zero then WriteRotation method is called.

WriteDirection is async method that takes no arguments. It writes Direction data to BLE direction characteristic using BleService's WriteDataAsync method.

WriteRotation is async method that takes no arguments. It writes Rotation data to BLE height characteristic using BleService's WriteDataAsync method.

WriteHeight is async method that takes no arguments. It writes Height data to BLE height characteristic using BleService's WriteDataAsync method.

EnableNotifications is async method that takes no arguments. It enables/disables notifications from BLE sensors characteristic based on property NotificationsEnables using BleService's RegisterForNotificationsAsync method. It passes lambda expression as delegate that calls ProcessReceivedData when data is received.

ProcessReceivedData method takes buffer byte array and int length as arguments. When called it adds buffer data to _receivedData MemoryStream. After adding data to _receivedData it searches for 0xAAAA marker that is used for synchronization of data. If marker has been found then position of _receivedData is set to marker's position and dataStartFound flag is set to true. Otherwise position is set to end of stream and dataStartFound is false. Next if dataStartFound flag is set and _receivedData contains enough bytes ($_receivedData.Length - _receivedData.Position > SizeOf(AnglesData) + 2$) then position of _receivedData is increased by 2 and AnglesData struct is read from stream. Parsed data is stored in AnglesString# property where # is number (1 - 6) depending on AnglesData.Leg value. Read data is removed from stream.

MovementJoystickMovedCommand is a command that takes Point as argument. When executed it calls OnMovementJoystickMoved delegate that stores Point X and Y coordinates to Direction X and Y properties. Then it calls WriteDirection and WriteRotation methods.

RotationJoystickMovedCommand is a command that takes Point as argument. When executed it calls OnRotationJoystickMoved delegate that stores Point X coordinate to Rotation property. Then it calls WriteDirection and WriteRotation methods.

ResetRobotCommand is a command that takes no arguments. When executed it calls OnResetRobot delegate that in turn calls ResetRobot method.

ResetRobot is async method that takes no arguments. It writes to BLE reset characteristic using BleService's WriteDataAsync method.

RcControlViewModel also Defines two structures: Vector2 that has public fields float X and float Y and AnglesData that is Sequential and Packed and contains public UInt32 Time, byte Leg and three float values Alpha, Beta and Gamma.


RcControlPage view consists of multiple rows and columns. In first row it has switch that enalbes and disables notifications, then six labels displaying received angles data and "Reset" button. Next row contains MovementJoystick, HeightSlider and RotationJoystick.

### Custom Joystick control
Because Xamarin doesn't provide Joystick control I had to implement custom control. Control has following BindableProperties:

JoystickWidth double, get, set. Lets you customize width of joystick.

JoystickHeight double, get, set. Lets you customize height of joystick.

HandleSize double, get, set. Lets you customize size of joystick handle.

BackgroundColor Color, get, set. Lets you customize color of joystick.

HandleColor Color, get, set. Lets you customize color of joystick handle.

Command ICommand, get, set. Lets you set command called when joystick is moved. It takes Point as argument.

JoystickMoved Event called when joystick is moved. It takes Point as argument.

Control uses PanGestureRecognizer to detect joystick movement. It uses PanGestureRecognizer_PanUpdated event handler to handle PanUpdated event. When called method maps movement X and Y coordinates to range from -1 to 1. Then it moves handle to event X and Y, calls JoystickMoved event and if Command is not null and CanExecute it executes Command.

## Additional resources
1. Xamarin BLE client - https://github.com/mo-thunderz/XamarinBleCodeBehind/
