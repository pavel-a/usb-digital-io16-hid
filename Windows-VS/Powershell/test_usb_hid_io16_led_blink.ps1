# Test for usb_io_interface.dll
# Calling from Powershell
#
# Powershell must be 32-bit to match the DLL's bitness
# (Original DLL is 32-bit only, ours is available both as 32 and 64 bit)
#
# Pointers, handles are passed as IntPtr (signed??, 32 or 64 bit)
# Enums are passed as Uint32 (unsigned?, always 32 bit)
# Returned status as Int32 (signed, always 32 bit)

# Detect script host bitness to use the matching DLL variant:
$host_arch = "x86"
if ( 8 -eq [IntPtr]::Size ) { $host_arch = "AMD64" }
"Host arch: {0}" -f $host_arch

$PathHere =  (Get-Location).Path

#########################################################################

function loadDll {

$PathUp = $PathHere.Substring(0, $PathHere.LastIndexOf("\"))

if (1) {
  # To use the DLL in the build tree
  # Add to PATH: for x86: ../Release ; for x64: ../x64/Release (or Debug, if you like)
  if ( $host_arch -eq "AMD64" ) { $DLL_path = "${PathUp}\x64\Release" } else { $DLL_path = "${PathUp}\Release" }
  $env:Path += ";" + $DLL_path
}

if (1) {
  # To use the DLL in the release tree
  # Add to PATH: for x86: ../bin-Win32 ; for x64: ../Win-x64
  if ( $host_arch -eq "AMD64" ) { $DLL_path = "${PathUp}\bin-Win64" } else { $DLL_path = "${PathUp}\bin-win32" }
  $env:Path += ";" + $DLL_path
} 

if (0) {
  # Copy usb_io_interface.dll here, or put it in PATH before running this script
  $env:Path += ";" + $PathHere
}


$DllName = "usb_io_interface.dll"
$Prefx = "[DllImport(`"${DllName}`")] public static extern "

$Methods_IOLIB = @"
$Prefx
System.Int32 usb_io_init();

$Prefx
System.Int32 usb_io_uninit();

$Prefx
System.IntPtr usb_io_get_device_list();

$Prefx
void usb_io_free_device_list(System.IntPtr devlist);

$Prefx
System.IntPtr usb_io_open_device(System.IntPtr develem);

$Prefx 
void usb_io_close_device(System.IntPtr hdev);

$Prefx 
System.Int32 usb_io_set_work_led_mode(System.IntPtr hdev, System.UInt32 onoff);

$Prefx 
System.Int32 usb_io_set_pin_mode(System.IntPtr hdev, 
        System.UInt32 pinIndex, System.UInt32 pinMode,
				System.UInt32 pullupMode);
				
$Prefx 
System.Int32 usb_io_write_output_pin_value(System.IntPtr hdev,
                 System.UInt32 pinIndex, System.Int32 pinLevel);

$Prefx 
System.Int32 usb_io16_lib_version();

"@

#.... TO DO define missing functions ....
# usb_io_read_input_pin_value - returns byref
# usb_io_get_all_pin_info - returns array of struct

#+ My additions:
# V usb_io16_lib_version
# usb_io_device_next_dev
# usb_io_device_get_id_string
# usb_io_device_open_with_serial_number


$Global:IOLIB = Add-Type -MemberDefinition $Methods_IOLIB -Name 'IOLIB' -Namespace 'MYAPP' -PassThru

# DLL functions can be called as [IOLIB]::funcname or [MYAPP.IOLIB]::funcname
# --------------

try {
  $libver = $IOLIB::usb_io16_lib_version()
  Write-Host "usb_io16_lib_version=" $libver
} catch {
  Write-Warning "usb_io16_lib_version() not found (using the original DLL?)"
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

} # init

##################################################################################

function test {

$devlist = $IOLIB::usb_io_get_device_list();
#echo $devlist

if ( $devlist.ToString() -eq "0" ) { 
  # DOESNT WORK: if ( $devlist -eq 0 ) ... just compare as strings!
  Write-Warning "No devices found, or all devices are opened"
  # Devices already opened do not even enumerate! how to fix this?
  return 1
} else {
  Write-Host "One or more device found..."

  # Open 1st detected device
  $dev = $IOLIB::usb_io_open_device($devlist)
  if ($dev.ToString() -ne "0") {
     Write-Host "Blinking LED..."
     $IOLIB::usb_io_set_work_led_mode( $dev, 1 );
     Sleep 3
     $IOLIB::usb_io_set_work_led_mode( $dev, 0 );
     $IOLIB::usb_io_close_device($dev)
     $dev = 0
  } else { Write-Warning "Error opening the device!" }
  
}

# Free the dev list:
$IOLIB::usb_io_free_device_list($devlist)
$devlist = 0

Write-Host "END test"

return 0
} #test_blink

# MAIN

loadDll

# "Now type: TEST" # Run: uncomment next line
test
