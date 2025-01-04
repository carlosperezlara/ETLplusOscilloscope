# ETLplusOscilloscope
Collection of code and scripts needed in order to run a Lecroy oscilloscope together with the ETL system test software

### Prerequisities
1. root
2. python with vxi11
3. ETL system test software
   
### Before you begin
To remote control the oscilloscope:
  1. Connect the lecroy oscilloscope into the network (VXI mode) and make sure you can ping on it
  2. Mount the oscilloscope output disk into the system. See mountLecroy.txt for details
  3. Test that you can remote-controlled the oscillosocpe. Run 'python3 reboot.py'
To convert and process the raw signals 
  1. Compile projects: readLecroy and SignalProcessing

### Main scripts:
Edit "cosmics.py" and repurpose it as needed.
Default configuration (from bottom to top):
  1. MCP - SiPM0+Scintillator - ETLModule - SiPM1+Scintillator - 2x2 LGAD
  2. Oscilloscope configuration: CH1 => SiPM0, CH2 => SiPM1, CH3 => MCP, CH4 => LGAD1, CH5 => LGAD2, CH6 => LGAD3, CH7 => LGAD4, CH8 => CLOCK
  3. Oscilloscope is trigger with channel 3 (MCP). Other trigger coincidences were tried, but this seems to be the optimal due to aperture of MCP.
  4. AuxOut is used as trigger-out and is send to KCU. 40Mhz Clock is received from KCU to oscilloscope channel 8.

Edit "takeRuns.sh" and repurpose it as needed.
Default configuration:
1. Runs an ETL baseline scan and sets the offset (currently to NW+1). Version1 PCB is quite noisy and this thereshold works, but lead to low efficiency. Once the noise is controlled, probably you want to set the threshold to NW/2+1 or so.
2. Loops over 30 runs with two steps
3. Step1: Launch ETL DAQ and waits for triggers
4. Step2: Launch Oscillsocope and ask for 1000 triggers
5. Repeat 1-4 for as many chuncks as needed
   
