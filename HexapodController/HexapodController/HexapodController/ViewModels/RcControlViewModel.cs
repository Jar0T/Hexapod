using HexapodController.Services;
using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace HexapodController.ViewModels
{
    public class RcControlViewModel : BaseViewModel
    {
        private IBleService _bleService => DependencyService.Get<IBleService>();
        private readonly IDevice _device;
        private readonly List<IService> _services = new List<IService>();
        private readonly List<ICharacteristic> _characteristics = new List<ICharacteristic>();
        private Vector2 _direction = new Vector2 { X = 0, Y = 0};
        private float _rotation = 0;
        private float _height = 100;
        public float Height
        {
            get { return _height; }
            set
            {
                if (_height != value)
                {
                    _height = value;
                    WriteHeight();
                }
            }
        }
        private bool _notificationsEnabled = false;
        public bool NotificationsEnabled
        {
            get { return _notificationsEnabled; }
            set
            {
                if (_notificationsEnabled != value)
                {
                    _notificationsEnabled = value;
                    EnableNotifications();
                }
            }
        }

        private string _anglesString1 = "";
        public string AnglesString1
        {
            get => _anglesString1;
            set => SetProperty(ref _anglesString1, value);
        }
        private string _anglesString2 = "";
        public string AnglesString2
        {
            get => _anglesString2;
            set => SetProperty(ref _anglesString2, value);
        }
        private string _anglesString3 = "";
        public string AnglesString3
        {
            get => _anglesString3;
            set => SetProperty(ref _anglesString3, value);
        }
        private string _anglesString4 = "";
        public string AnglesString4
        {
            get => _anglesString4;
            set => SetProperty(ref _anglesString4, value);
        }
        private string _anglesString5 = "";
        public string AnglesString5
        {
            get => _anglesString5;
            set => SetProperty(ref _anglesString5, value);
        }
        private string _anglesString6 = "";
        public string AnglesString6
        {
            get => _anglesString6;
            set => SetProperty(ref _anglesString6, value);
        }

        private MemoryStream _receivedData = new MemoryStream();

        private CancellationTokenSource _cancellationTokenSource = new CancellationTokenSource();

        public Command<Point> MovementJoystickMovedCommand { get; }
        public Command<Point> RotationJoystickMovedCommand { get; }
        public Command ResetRobotCommand {  get; }

        public RcControlViewModel(IDevice device)
        {
            _device = device ?? throw new ArgumentNullException(nameof(device));
            LoadServicesAndCharacteristics();
            MovementJoystickMovedCommand = new Command<Point>(OnMovementJoystickMoved);
            RotationJoystickMovedCommand = new Command<Point>(OnRotationJoystickMoved);
            ResetRobotCommand = new Command(OnResetRobot);
            Task.Run(async () => await WriteMovementData());
        }

        private async void LoadServicesAndCharacteristics()
        {
            await LoadServices();
            await LoadCharacteristics();
        }

        private async Task LoadServices()
        {
            _services.Clear();
            _services.AddRange(await _bleService.GetServicesAsync(_device));

            Guid guid = new Guid("00000001-61d1-11ee-8c99-0242ac120002");
            if (!_services.Any((service) => service.Id == guid))
            {
                //TODO: display warning and pop back to ble devices page
                await _bleService.DisconnectFromDeviceAsync(_device);
                await Application.Current.MainPage.Navigation.PopAsync();
            }
        }

        private async Task LoadCharacteristics()
        {
            _characteristics.Clear();
            foreach (var service in _services)
            {
                _characteristics.AddRange(await _bleService.GetCharacteristicsAsync(service));
            }
        }

        private async Task WriteMovementData()
        {
            while (!_cancellationTokenSource.IsCancellationRequested)
            {
                if (_direction.X != 0 || _direction.Y != 0) WriteDirection();
                if (_rotation != 0) WriteRotation();
                await Task.Delay(TimeSpan.FromMilliseconds(100), _cancellationTokenSource.Token);
            }
        }

        private async void WriteDirection()
        {
            Guid charGuid = new Guid("00000002-61d1-11ee-8c99-0242ac120002");
            int size = Marshal.SizeOf(_direction);
            byte[] data = new byte[size];
            IntPtr ptr = IntPtr.Zero;
            try
            {
                ptr = Marshal.AllocHGlobal(size);
                Marshal.StructureToPtr(_direction, ptr, true);
                Marshal.Copy(ptr, data, 0, size);
                await _bleService.WriteDataAsync(
                    _characteristics.FirstOrDefault((character) => character.Id == charGuid),
                    data
                    );
            }
            catch
            {

            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        private async void WriteRotation()
        {
            Guid charGuid = new Guid("00000003-61d1-11ee-8c99-0242ac120002");
            await _bleService.WriteDataAsync(
                _characteristics.FirstOrDefault((character) => character.Id == charGuid),
                BitConverter.GetBytes(_rotation)
                );
        }

        private async void WriteHeight()
        {
            Guid charGuid = new Guid("00000004-61d1-11ee-8c99-0242ac120002");
            await _bleService.WriteDataAsync(
                _characteristics.FirstOrDefault((character) => character.Id == charGuid),
                BitConverter.GetBytes(_height)
                );
        }

        private async void EnableNotifications()
        {
            Guid characterGuid = new Guid("00000002-2ec8-467d-901b-5e81822a6ffe");
            await _bleService.RegisterForNotificationsAsync(
                _characteristics.FirstOrDefault((characteristic) => characteristic.Id == characterGuid),
                _notificationsEnabled,
                (characteristic) => {
                    byte[] buffer = characteristic.Value;
                    ProcessReceivedData(buffer, buffer.Length);
                }
                );
            if ( _notificationsEnabled ) { _receivedData = new MemoryStream(); }
        }

        private void ProcessReceivedData(byte[] buffer, int length)
        {
            _receivedData.Write(buffer, 0, length);
            
            bool dataStartFound = false;
            byte[] data = new byte[2];

            for (int i = 0 ; i < _receivedData.Length - 1; i++)
            {
                _receivedData.Position = i;
                _receivedData.Read(data, 0, 2);
                if (data[0] == 0xaa && data[1] == 0xaa)
                {
                    _receivedData.Position = i;
                    dataStartFound = true;
                    break;
                }
            }

            if (dataStartFound)
            {
                AnglesData anglesData = new AnglesData();
                while (_receivedData.Length - _receivedData.Position >= Marshal.SizeOf(anglesData) + 2)
                {
                    _receivedData.Position += 2;

                    data = new byte[Marshal.SizeOf(anglesData)];
                    _receivedData.Read(data, 0, data.Length);

                    GCHandle handle = GCHandle.Alloc(data, GCHandleType.Pinned);
                    try
                    {
                        anglesData = (AnglesData)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(AnglesData));
                        double converter = 180 / Math.PI;
                        string angles = $"A: {anglesData.Alpha * converter:0.00}\nB: {anglesData.Beta * converter:0.00}\nC: {anglesData.Gamma * converter:0.00}";
                        switch (anglesData.Leg)
                        {
                            case 0:
                                AnglesString1 = angles;
                                break;
                            case 1:
                                AnglesString2 = angles;
                                break;
                            case 2:
                                AnglesString3 = angles;
                                break;
                            case 3:
                                AnglesString4 = angles;
                                break;
                            case 4:
                                AnglesString5 = angles;
                                break;
                            case 5:
                                AnglesString6 = angles;
                                break;
                            default:
                                break;
                        }
                    }
                    finally
                    {
                        handle.Free();
                    }

                    MemoryStream newBuffer = new MemoryStream();
                    _receivedData.CopyTo(newBuffer);
                    _receivedData = newBuffer;
                    _receivedData.Position = 0;
                }
            }
        }

        private async void ResetRobot()
        {
            Guid charGuid = new Guid("00000002-eeaa-4c7d-9a63-8f41e0f2d3a7");
            await _bleService.WriteDataAsync(
                _characteristics.FirstOrDefault((character) => character.Id == charGuid),
                new byte[] { 1 }
                );
        }

        private void OnMovementJoystickMoved(Point point)
        {
            _direction.X = (float)point.X;
            _direction.Y = (float)point.Y;
            WriteDirection();
            WriteRotation();
        }

        private void OnRotationJoystickMoved(Point point)
        {
            _rotation = (float)point.X;
            WriteDirection();
            WriteRotation();
        }

        private void OnResetRobot()
        {
            ResetRobot();
        }

        private struct Vector2
        {
            public float X { get; set; }
            public float Y { get; set; }
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        private struct AnglesData
        {
            public UInt32 Time { get; set; }
            public byte Leg { get; set; }
            public float Alpha {  get; set; }
            public float Beta {  get; set; }
            public float Gamma {  get; set; }
        }
    }
}
