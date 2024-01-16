using HexapodController.Services;
using HexapodController.Views;
using Plugin.BLE;
using Plugin.BLE.Abstractions;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Essentials;
using Xamarin.Forms;

namespace HexapodController.ViewModels
{
    public class BleDevicesViewModel : BaseViewModel
    {
        private IBleService _bleService => DependencyService.Get<IBleService>();
        private readonly IAdapter bleAdapter;
        private bool scanEnabled = true;
        public ObservableCollection<IDevice> BleDevices { get; }
        public bool ScanEnabled
        {
            get => scanEnabled;
            set => SetProperty(ref scanEnabled, value);
        }
        public Command BleScanCommand { get; }
        public Command<IDevice> DeviceSelectedCommand { get; }

        public BleDevicesViewModel()
        {
            Title = "BLE Devices";
            BleDevices = new ObservableCollection<IDevice>();

            BleScanCommand = new Command(OnBleScan, BleScanCanExecute);
            DeviceSelectedCommand = new Command<IDevice>(OnDeviceSelected);

            bleAdapter = CrossBluetoothLE.Current.Adapter;
        }

        private async Task<bool> PermissionGrantedAsync()
        {
            var status = await Permissions.CheckStatusAsync<Permissions.LocationWhenInUse>();

            if (status != PermissionStatus.Granted)
            {
                status = await Permissions.RequestAsync<Permissions.LocationWhenInUse>();
            }

            return status == PermissionStatus.Granted;
        }

        private async void OnBleScan()
        {
            ScanEnabled = false;

            if (!await PermissionGrantedAsync())
            {
                ScanEnabled = true;
                return;
            }

            BleDevices.Clear();

            var devices = await _bleService.GetDevicesAsync();

            foreach (var device in devices)
            {
                BleDevices.Add(device);
            }

            ScanEnabled = true;
        }

        private bool BleScanCanExecute() { return scanEnabled; }

        async private void OnDeviceSelected(IDevice device)
        {
            var page = new RcControlPage();
            if (device.State != DeviceState.Connected)
            {
                try
                {
                    ConnectParameters connectParameters = new ConnectParameters(false, true);
                    await bleAdapter.ConnectToDeviceAsync(device, connectParameters);
                }
                catch
                {
                    //TODO display error
                    return;
                }
            }
            var viewModel = new RcControlViewModel(device);
            page.BindingContext = viewModel;
            await Application.Current.MainPage.Navigation.PushAsync(page);
        }
    }
}
