﻿using Plugin.BLE;
using Plugin.BLE.Abstractions;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE.Abstractions.EventArgs;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace HexapodController.Services
{
    public class BleService : IBleService
    {
        private readonly IAdapter _bleAdapter;
        private readonly List<IDevice> _devices;

        public BleService()
        {
            _bleAdapter = CrossBluetoothLE.Current.Adapter;
            _bleAdapter.DeviceDiscovered += bleAdapter_DeviceDiscovered;
            _devices = new List<IDevice>();
        }

        private void bleAdapter_DeviceDiscovered(object sender, DeviceEventArgs e)
        {
            if (e.Device != null && !string.IsNullOrEmpty(e.Device.Name))
                _devices.Add(e.Device);
        }

        public async Task<IEnumerable<IDevice>> GetDevicesAsync()
        {
            _devices.Clear();

            if (!_bleAdapter.IsScanning)
                await _bleAdapter.StartScanningForDevicesAsync();

            return _devices;
        }

        public async Task ConnectToDeviceAsync(IDevice device)
        {
            ConnectParameters connectParameters = new ConnectParameters(false, true);
            await _bleAdapter.ConnectToDeviceAsync(device, connectParameters);
        }

        public async Task DisconnectFromDeviceAsync(IDevice device)
        {
            await _bleAdapter.DisconnectDeviceAsync(device);
        }
    }
}
