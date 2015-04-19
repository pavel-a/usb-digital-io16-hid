# Test for usb_io_interface.dll
# Calling from Powershell
#
# Powershell must be 32-bit to match the DLL's bitness
# (Original DLL is 32-bit only, ours is available both as 32 and 64 bit)
#
# Pointers, handles are passed as IntPtr (signed??, 32 or 64 bit)
# Enums are passed as Uint32 (unsigned?, always 32 bit)
# Returned status as Int32 (signed, always 32 bit)

$Methods_IOLIB = @'

[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io_init();

[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io_uninit();

[DllImport("usb_io_interface.dll")]
public static extern System.IntPtr usb_io_get_device_list();

[DllImport("usb_io_interface.dll")]
public static extern void usb_io_free_device_list(System.IntPtr devlist);

[DllImport("usb_io_interface.dll")]
public static extern System.IntPtr usb_io_open_device(System.IntPtr develem);

[DllImport("usb_io_interface.dll")]
public static extern void usb_io_close_device(System.IntPtr hdev);

[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io_set_work_led_mode(System.IntPtr hdev, System.UInt32 onoff);

[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io_set_pin_mode(System.IntPtr hdev, 
                System.UInt32 pinIndex, System.UInt32 pinMode,
				System.UInt32 pullupMode);
				
[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io_write_output_pin_value(System.IntPtr hdev,
                 System.UInt32 pinIndex, System.Int32 pinLevel);

[DllImport("usb_io_interface.dll")]
public static extern System.Int32 usb_io16_lib_version();


//.... TO DO define missing functions ....
// usb_io_read_input_pin_value - returns byref?
// usb_io_get_all_pin_info - returns array of struct

//+ My additions:
// V usb_io16_lib_version
// usb_io_device_next_dev
// usb_io_device_get_id_string
// usb_io_device_open_with_serial_number

'@

function test {

$IOLIB = Add-Type -MemberDefinition $Methods_IOLIB -Name 'IOLIB' -Namespace 'MYAPP' -PassThru

# functions can be also called as [MYAPP.IOLIB]::funcname
# --------------

try {
  $libver = $IOLIB::usb_io16_lib_version()
  Write-Host "usb_io16_lib_version=" $libver
} catch {
  Write-Warning "usb_io16_lib_version() not found (using the original DLL?)"
}

try {
  $libver = [MYAPP.IOLIB]::usb_io16_lib_version()
  Write-Host "2usb_io16_lib_version=" $libver
} catch {
  Write-Warning "2usb_io16_lib_version() not found (using the original DLL?)"
}

try {
  if ( 0 -ne $IOLIB::usb_io_init() ) {
     Write-Warning "Error init usb_io lib"
     throw "error init"
  }
} catch {
     Write-Warning "Error calling usb_io lib"
     return 1
}

$devlist = $IOLIB::usb_io_get_device_list();
echo $devlist

if ( $devlist.ToString() -eq "0" ) { 
  # DOESNT WORK: if ( $devlist -eq "0" ) { 
  Write-Warning "No devices found"
  return 1
}
else {
  Write-Host "Some devices found, list=", $devlist

  # Open 1st detected device
  $dev = $IOLIB::usb_io_open_device($devlist)
  if ($dev -ne 0) {
     Write-Host "Blinking LED..."
     $IOLIB::usb_io_set_work_led_mode( $dev, 1 );
     sleep 3
     $IOLIB::usb_io_set_work_led_mode( $dev, 0 );
     $IOLIB::usb_io_close_device($dev)
     $dev = 0
  }
}

  # Free the dev list:
  $IOLIB::usb_io_free_device_list($devlist)
  $devlist = 0


Write-Host "END"

return 0
}

