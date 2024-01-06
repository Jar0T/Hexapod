using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using Xamarin.Forms;

namespace HexapodController.ViewModels
{
    public class RcControlViewModel : BaseViewModel
    {
        private readonly IDevice _device;
        private readonly List<IService> _services = new List<IService>();
        private readonly List<ICharacteristic> _characteristics = new List<ICharacteristic>();
        private RCData _rcData;

        public Command<Point> MovementJoystickMovedCommand { get; }
        public Command<Point> RotationJoystickMovedCommand { get; }

        public RcControlViewModel(IDevice device)
        {
            _device = device ?? throw new ArgumentNullException(nameof(device));
            LoadServices();
            _rcData = new RCData
            {
                X = 0,
                Y = 0,
                FI = 0,
                H = 100
            };
            MovementJoystickMovedCommand = new Command<Point>(OnMovementJoystickMoved);
            RotationJoystickMovedCommand = new Command<Point>(OnRotationJoystickMoved);
        }

        private async void LoadServices()
        {
            try
            {
                var servicesReadOnly = await _device.GetServicesAsync();
                _services.Clear();
                _characteristics.Clear();
                foreach (var service in servicesReadOnly)
                {
                    _services.Add(service);
                    LoadCharacteristics(service);
                }
                Guid guid = new Guid("00000001-61d1-11ee-8c99-0242ac120002");
                if (!_services.Any((service) => service.Id == guid))
                {
                    //TODO: display warning and pop back to ble devices page
                    await Application.Current.MainPage.Navigation.PopAsync();
                }
            }
            catch
            {
                //TODO: display warning
            }
        }

        private async void LoadCharacteristics(IService service)
        {
            try
            {
                var characteristicsReadOnly = await service.GetCharacteristicsAsync();
                foreach (var characteristic in characteristicsReadOnly)
                {
                    _characteristics.Add(characteristic);
                }
            }
            catch
            {
                //TODO: display warning
            }
        }

        private async void WriteRCData()
        {
            Guid charGuid = new Guid("00000002-61d1-11ee-8c99-0242ac120002");
            ICharacteristic characteristic = _characteristics.First((character) => character.Id == charGuid);
            if (characteristic.CanWrite)
            {
                int size = Marshal.SizeOf(_rcData);
                byte[] data = new byte[size];
                IntPtr ptr = IntPtr.Zero;
                try
                {
                    ptr = Marshal.AllocHGlobal(size);
                    Marshal.StructureToPtr(_rcData, ptr, true);
                    Marshal.Copy(ptr, data, 0, size);
                    await characteristic.WriteAsync(data);
                }
                catch
                {

                }
                finally
                {
                    Marshal.FreeHGlobal(ptr);
                }
            }
        }

        private void OnMovementJoystickMoved(Point point)
        {
            _rcData.X = (float)point.X;
            _rcData.Y = (float)point.Y;
            WriteRCData();
        }

        private void OnRotationJoystickMoved(Point point)
        {
            _rcData.FI = (float)point.X;
            WriteRCData();
        }

        private struct RCData
        {
            public float X { get; set; }
            public float Y { get; set; }
            public float FI { get; set; }
            public float H { get; set; }
        }
    }
}
