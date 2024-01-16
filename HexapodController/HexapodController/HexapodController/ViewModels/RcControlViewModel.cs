using HexapodController.Services;
using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
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
            get
            {
                return _height;
            }
            set
            {
                if (_height != value)
                {
                    _height = value;
                    WriteHeight();
                }
            }
        }

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
    }
}
