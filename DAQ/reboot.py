import oscilloscopes

_IP = "192.168.0.169"
inst = oscilloscopes.lecroy( _IP, timeout=10 ) #timeout in seconds

inst.clearAll()
