"""
Simple test for the USB IO16 interface DLL
Just plain calling the C library, no fancy OOP stuff
Uses CTYPES

For python2, 3
"""
import sys, os, time
import ctypes

print("Running on Python v." + str(sys.version))
print( {4:32, 8:64}[ctypes.sizeof(ctypes.c_void_p)], "-bit mode")

if sys.version_info.major >= 3:
  def charpToString(charp):
     return str(ctypes.string_at(charp), 'ascii')
  def stringToCharp(s) :   
    return bytes(s, "ascii")
else:
  def charpToString(charp) :
     return str(ctypes.string_at(charp))
  def stringToCharp(s) :   
    return bytes(s)  #bytes(s, "ascii")
 

libpath=r'.' + "\\"
libfile = {'nt':   "usb_io_interface.dll", 
           'posix': "usb_hid_io16.so",
           'darwin':"usb_hid_io16.dylib",
           } [os.name]

#?? MAC => os.name == "posix" and sys.platform == "darwin"

devids = []
hdev = None
g_enumlist = None

def exc(msg):  return Exception(msg)

def fail(msg) : raise exc(msg)
 
class L: pass   # Global object for the DLL
setattr(L, "dll", None)

def loadLib():
  # Load the C DLL ...
  if not L.dll :
    print("Loading DLL: %s" % (libpath + libfile))
    try:
      L.dll = ctypes.CDLL( libpath + libfile)
    except OSError:  
      fail("Failed load lib")
  else:
    print("lib already open")
  #print(L.dll)

  
usb_relay_lib_funcs = [
  # TYpes: h=handle (pointer sized), p=pointer, i=int, e=error num (int), s=string
  ("usb_io_get_device_list",               'p', None),
  ("usb_io_open_device",                   'h', 'p'),
  ("usb_io_close_device",                  'e', 'h'),
  ("usb_io_free_device_list",              'i', 'p'),
  ("usb_io_set_work_led_mode",             'e', 'hi'),
  ("usb_io_device_open_with_serial_number", 'h', 'ps'),
  ("usb_io_device_get_id_string",       's', 'h'),
  ("usb_io_device_next_dev",            'h', 'h'),
  #...............
  ]
      
      
def getLibFunctions():
  """ Get needed functions and configure types; call lib. init.
  """
  assert L.dll
  
  #Get lib version (my extension, not in the original dll)
  libver = L.dll.usb_io16_lib_version()  
  print("%s version: 0x%X" % (libfile,libver))
  
  ret = L.dll.usb_io_init()
  if ret != 0 : fail("Failed lib init!")
  
  """
  Make prototypes for imported C functions
  This is required in 64-bit mode. Optional for 32-bit (when pointer size=int size)
  Functions that return and receive ints or void work without specifying types.
  """
  ctypemap = { 'e': ctypes.c_int, 'h':ctypes.c_void_p, 'p': ctypes.c_void_p,
            'i': ctypes.c_int, 's': ctypes.c_char_p}
  for x in usb_relay_lib_funcs :
      fname, ret, param = x
      try:
        f = getattr(L.dll, fname)
      except Exception:  
        fail("Missing lib export:" + fname)

      ps = []
      if param :
        for p in param :
          ps.append( ctypemap[p] )
      f.argtypes = ps
      f.restype = ctypemap[ret]
      setattr(L, fname, f)


def openDev() :
  assert(g_enumlist)
  h = L.usb_io_open_device(g_enumlist) # 1st dev
  if not h: fail("Cannot open device!")
  global hdev
  hdev = h  
      
def openDevById(idstr):
  #Open by known ID:
  print("Opening " + idstr)
  h = L.usb_io_device_open_with_serial_number(g_enumlist, stringToCharp(idstr))
  if not h: fail("Cannot open device with id="+idstr)
  global hdev
  hdev = h  

def closeDev():
  global hdev
  L.usb_io_close_device(hdev)
  hdev = None

def enumDevs():
  global devids, g_enumlist
  devids = []
  enuminfo = L.usb_io_get_device_list()
  g_enumlist = enuminfo
  print(g_enumlist)
  while enuminfo :
    idstrp = L.usb_io_device_get_id_string(enuminfo)
    idstr = charpToString(idstrp)
    print(idstr)
    assert len(idstr) == 4
    if not idstr in devids : devids.append(idstr)
    else : print("Warning! found duplicate ID=" + idstr)
    enuminfo = L.usb_io_device_next_dev(enuminfo)

  print("Found devices: %d" % len(devids))
  
def unloadLib():
  global hdev, L
  if hdev: closeDev()
  if g_enumlist: L.usb_io_free_device_list(g_enumlist)
  L.dll.usb_io_uninit()
  L.dll = None
  print("Lib closed")
  

def testLed() :
  print("Blinking green LED on...")
  assert(hdev)
  L.usb_io_set_work_led_mode(hdev, 1)
  time.sleep(1)
  L.usb_io_set_work_led_mode(hdev, 0)
  print("Blinking green LED off")
  
  
# main
def main():
  loadLib()
  getLibFunctions()
  try:
    print("Searching for compatible devices")
    enumDevs()
    if len(devids) != 0 :
      # Test any 1st found dev .
      openDev();
      testLed();
      closeDev();
  finally:  
    unloadLib()
  print("end.")
 
    #    if len(devids) != 0 :
      # Test any 1st found dev .
#      print("Testing relay with ID=" + devids[0])
#      openDevById(g_enumlist, devids[0])
#      testR2()
#      closeDev()
  
print("START 0, name=%s" % (__name__))
if __name__ == "__main__" :
  print("Loading C lib from " + libpath + " ...")
  main()
