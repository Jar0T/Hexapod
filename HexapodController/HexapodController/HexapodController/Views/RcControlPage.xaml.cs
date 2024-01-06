using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace HexapodController.Views
{
	[XamlCompilation(XamlCompilationOptions.Compile)]
	public partial class RcControlPage : ContentPage
	{
		public RcControlPage()
		{
			InitializeComponent ();
		}
    }
}