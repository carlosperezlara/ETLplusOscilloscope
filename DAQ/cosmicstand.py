from oscilloscopes import lecroy

def GetNextIndex():
    run_num_file = "./runningIndex.txt"
    FileHandle = open(run_num_file)
    nextNumber = int(FileHandle.read().strip())
    FileHandle.close()
    FileHandle = open(run_num_file,"w")
    FileHandle.write(str(nextNumber+1)+"\n") 
    FileHandle.close()
    return nextNumber


_EVENTS = 100
_IP = "192.168.0.169"
_FILESUFFIX = "Cosmics_{}".format( GetNextIndex() )

debug=False

#INIT
inst = lecroy( _IP, timeout=_EVENTS*10*60 ) #timeout in seconds
inst.clearAll()

#WINDOW SETUP
#inst.setTime( hscale=5e-9, hdel=0, debug=debug )
inst.setTime( hscale=5e-9, hdel=-5e-9, debug=debug )
inst.setChannel( chn="C1", vscale=0.200, voff=+0.600, debug=debug ) # SiPM 1
inst.setChannel( chn="C2", vscale=0.200, voff=+0.600, debug=debug ) # SiPM 2
inst.setChannel( chn="C3", vscale=0.200, voff=+0.600, debug=debug ) # MCP
inst.setChannel( chn="C4", vscale=0.100, voff=+0.300, debug=debug ) # LGAD1
inst.setChannel( chn="C5", vscale=0.100, voff=+0.300, debug=debug ) # LGAD2
inst.setChannel( chn="C6", vscale=0.100, voff=+0.300, debug=debug ) # LGAD3
inst.setChannel( chn="C7", vscale=0.100, voff=+0.300, debug=debug ) # LGAD4
inst.setChannel( chn="C8", vscale=0.200, voff=-0.600, debug=debug ) # ETLCLK

#TRIGGER SETUP
inst.setTriggerLevel( chn="C1", slope="NEG", lev=-0.120, debug=debug )
inst.setTriggerLevel( chn="C2", slope="NEG", lev=-0.120, debug=debug )
inst.setTriggerLevel( chn="C3", slope="NEG", lev=-0.100, debug=debug )

inst.setTriggerEdge( src="C3", debug=False )
#inst.setTriggerPattern( src="C1,L,C2,L", con="AND", debug=debug )
#inst.setTriggerMultiStage( debug=debug )
#inst.setAuxOuttoTriggerTTL()

#ACQUIRE
inst.acquire( cnt=_EVENTS, fileSuffix=_FILESUFFIX )
