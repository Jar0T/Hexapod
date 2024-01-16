using Plugin.BLE.Abstractions.Contracts;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace HexapodController.Services
{
    public interface IBleService
    {
        Task<IEnumerable<IDevice>> GetDevicesAsync();
        Task ConnectToDeviceAsync(IDevice device);
        Task DisconnectFromDeviceAsync(IDevice device);
        Task<IEnumerable<IService>> GetServicesAsync(IDevice device);
        Task<IEnumerable<ICharacteristic>> GetCharacteristicsAsync(IService service);
    }
}
