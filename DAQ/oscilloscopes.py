import vxi11

class lecroy:
    def __init__(self, address, timeout=5):
        self.instr = vxi11.Instrument( address )
        #self.instr.write("vbs 'app.settodefaultsetup'")
        self.instr.timeout = timeout
   
    def __del__(self):
        #self.instr.write("vbs 'app.settodefaultsetup'")
        self.instr.close()
 
    def clearAll(self):
        self.instr.clear()
        self.instr.write("*CLS")
        #self.instr.write("*RST")
        self.instr.write("ALST?")
        self.instr.write( "TRIG_MODE STOP" )
        print( self.instr.ask('*IDN?') )
        self.instr.write("GRID SINGLE")
        self.instr.write("DISPLAY ON")

    def setTriggerLevel( self, chn="C1", slope="POS", lev=0.1, debug=False ): ### units V
        self.instr.write( "{}:TRIG_LEVEL {}".format(chn,lev) )
        self.instr.write( "{}:TRIG_SLOPE {}".format(chn,slope) )
        if(debug):
            print( "Trigger level:", self.instr.ask("{}:TRIG_LEVEL?".format(chn)) )
            print( "Trigger slope:", self.instr.ask("{}:TRIG_SLOPE?".format(chn)) )
    
    def setTriggerEdge( self, src="C1", debug=False ): ### units V
        self.instr.write( "TRIG_SELECT Edge,SR,{}".format(src) )
        self.instr.write( "TRIG_MODE SINGLE" )
        if(debug):
            print( "Trigger mode:", self.instr.ask("TRIG_MODE?") )
    
    def setTriggerPattern( self, src="C1,H,C2,H", con="OR", debug=False ): ### units V
        self.instr.write( "TRIG_SELECT PA" )
        self.instr.write( "TRIG_PATTERN {},STATE,{}".format(src,con) )
        if(debug):
            print( "Trigger mode:", self.instr.ask("TRIG_MODE?") )
    
    def setTriggerMultiStage( self,  debug=False ): ### units V
        self.instr.write( "TRIG_SELECT TEQ,SR,C1,QL,C2,HT,TL,HV,10e-9S" )
        if(debug):
            print( "Trigger mode:", self.instr.ask("TRIG_MODE?") )
    
    def setChannel( self, chn="C1", vscale=50e-3, voff=0.0, debug=False ): ### units V
        self.instr.write( "{}:COUPLING D50".format(chn) )
        self.instr.write( "{}:VOLT_DIV {}".format(chn,vscale) )
        self.instr.write( "{}:OFFSET {}".format(chn,voff) )
        self.instr.write( "{}:TRACE ON".format(chn) )
        if(debug):
            print( "{} scale:".format(chn), self.instr.ask( "{}:VOLT_DIV?".format(chn) ) )
            print( "{} offset:".format(chn), self.instr.ask( "{}:OFFSET?".format(chn) ) )

    def setTime( self, hscale=0.0002, hdel=0.0, debug=False ): ### units seconds
        self.instr.write( "TIME_DIV {}".format(hscale) )
        self.instr.write( "TRIG_DELAY {}".format(hdel) )
        if(debug):
            print( "Timebase scale:", self.instr.ask( "TIME_DIV?" ) )
            print( "Timebase offset:", self.instr.ask( "TRIG_DELAY?" ) )

    #def setAuxOuttoTriggerTTL(self):
        #self.instr.write( "vbs 'app.Acquisition.AuxOutput.Mode = \"TriggerOut\"'" )
        #self.instr.write( "vbs 'app.Acquisition.AuxOutput.Mode=\"PassFail\"'" )
        #self.instr.write( "vbs 'app.Acquisition.AuxOutput.SetToTTL = \"True\"'" )
        
    def acquire( self, cnt=1000, fileSuffix="tmp" ):
        print("START ACQUISITION FOR RUN {} => {} events".format(fileSuffix,cnt))
        self.instr.write( "TRIG_MODE STOP" )
        self.instr.write( "DISPLAY OFF" )
        self.instr.write( "STORE_SETUP ALL_DISPLAYED,HDD,AUTO,OFF,FORMAT,BINARY" )
        self.instr.write( "SEQ ON, {}".format(cnt) )
        self.instr.write( "WAIT" )
        self.instr.ask( "*OPC?" )
        self.instr.write( "TRIG_MODE SINGLE" )
        self.instr.write( "ARM" )
        self.instr.write( "WAIT" )
        self.instr.ask( "*OPC?" )
        self.instr.write( "TRIG_MODE STOP" )
        self.instr.write( "vbs 'app.SaveRecall.Waveform.TraceTitle=\"{}\"'".format(fileSuffix) )
        self.instr.write( "vbs 'app.SaveRecall.Waveform.SaveFile'" )
        self.instr.write( "WAIT" )
        self.instr.ask( "*OPC?" )
        print("FINISHED SUCCESSFULLY")

