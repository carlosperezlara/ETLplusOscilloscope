pattern="80901810"
server="daq@timingdaq03.dhcp.fnal.gov"
sourcetrc="/home/daq/LecroyMount/"
sourcedat="/home/daq/ETROC2_Test_Stand/module_test_sw/ETROC_output/"

echo "rsync -azvh  --include='output_run_*${pattern}*.dat' --exclude='*' ${server}:${sourcedat} dat/"
echo "rsync -azvh  --include='*${pattern}*.trc' --exclude='*' ${server}:${sourcetrc} trc/"
