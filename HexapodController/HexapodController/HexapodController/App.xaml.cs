using HexapodController.Services;
using HexapodController.Views;
using System;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace HexapodController
{
    public partial class App : Application
    {
        public static INavigation GlobalNavigation { get; private set; }

        public App()
        {
            InitializeComponent();

            DependencyService.Register<MockDataStore>();
            DependencyService.Register<BleService>();

            var rootPage = new NavigationPage(new BleDevicesPage());
            GlobalNavigation = rootPage.Navigation;
            MainPage = rootPage;
            //MainPage = new AppShell();
        }

        protected override void OnStart()
        {
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
