using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace HexapodController.CustomControls
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class VirtualJoystick : ContentView
    {
        public static readonly BindableProperty JoystickWidthProperty = 
            BindableProperty.Create(
                nameof(JoystickWidth),
                typeof(double),
                typeof(VirtualJoystick),
                100.0,
                propertyChanged: JoystickWidthPropertyChanged);

        private static void JoystickWidthPropertyChanged(BindableObject bindable, object oldValue, object newValue)
        {
            VirtualJoystick joystick = (VirtualJoystick)bindable;
            joystick.Layout.WidthRequest = (double)newValue;
        }

        public double JoystickWidth
        { 
            get => (double)GetValue(JoystickWidthProperty);
            set => SetValue(JoystickWidthProperty, value);
        }

        public static readonly BindableProperty JoystickHeightProperty =
            BindableProperty.Create(
                nameof(JoystickHeight),
                typeof(double),
                typeof(VirtualJoystick),
                100.0,
                propertyChanged: JoystickHeightPropertyChanged);

        private static void JoystickHeightPropertyChanged(BindableObject bindable, object oldValue, object newValue)
        {
            VirtualJoystick joystick = (VirtualJoystick)(bindable);
            joystick.Layout.HeightRequest = (double)newValue;
        }

        public double JoystickHeight
        {
            get => (double)GetValue(JoystickHeightProperty);
            set => SetValue(JoystickHeightProperty, value);
        }

        public static readonly BindableProperty HandleSizeProperty =
            BindableProperty.Create(
                nameof(HandleSize),
                typeof(double),
                typeof(VirtualJoystick),
                40.0,
                propertyChanged: HandleSizePropertyChanged);

        private static void HandleSizePropertyChanged(BindableObject bindable, object oldValue, object newValue)
        {
            VirtualJoystick joystick = (VirtualJoystick)(bindable);
            joystick.Handle.HeightRequest = (double)newValue;
            joystick.Handle.WidthRequest = (double)newValue;
            joystick.Handle.CornerRadius = (double)newValue / 2;
        }

        public double HandleSize
        {
            get => (double)GetValue(HandleSizeProperty);
            set => SetValue(HandleSizeProperty, value);
        }

        public static new readonly BindableProperty BackgroundColorProperty =
            BindableProperty.Create(
                nameof(BackgroundColor),
                typeof(Color),
                typeof(VirtualJoystick),
                Color.Gray,
                propertyChanged: BackgroundColorPropertyChanged);

        private static void BackgroundColorPropertyChanged(BindableObject bindable, object oldValue, object newValue)
        {
            VirtualJoystick joystick = (VirtualJoystick)(bindable);
            joystick.Background.BackgroundColor = (Color)newValue;
        }

        public new Color BackgroundColor
        {
            get => (Color)GetValue(BackgroundColorProperty);
            set => SetValue(BackgroundColorProperty, value);
        }

        public static readonly BindableProperty HandleColorProperty =
            BindableProperty.Create(
                nameof(HandleColor),
                typeof(Color),
                typeof(VirtualJoystick),
                Color.Red,
                propertyChanged: HandleColorPropertyChanged);

        private static void HandleColorPropertyChanged(BindableObject bindable, object oldValue, object newValue)
        {
            VirtualJoystick joystick = (VirtualJoystick)(bindable);
            joystick.Handle.Color = (Color)newValue;
        }

        public Color HandleColor
        {
            get => (Color)GetValue(HandleColorProperty);
            set => SetValue(HandleColorProperty, value);
        }

        public event EventHandler<Point> JoystickMoved;

        public VirtualJoystick()
        {
            InitializeComponent();
        }

        private void PanGestureRecognizer_PanUpdated(object sender, PanUpdatedEventArgs e)
        {
            double x = e.TotalX / Layout.Width;
            double y = e.TotalY / Layout.Height;

            // clamp x and y to be between -1 and 1
            x = Math.Max(-0.5, Math.Min(x, 0.5));
            y = Math.Max(-0.5, Math.Min(y, 0.5));

            AbsoluteLayout.SetLayoutBounds(Handle, new Rectangle(0.5 + x, 0.5 + y, HandleSize, HandleSize));

            JoystickMoved?.Invoke(this, new Point(2 * x, 2 * y));
        }
    }
}