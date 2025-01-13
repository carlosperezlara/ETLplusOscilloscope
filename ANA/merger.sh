PREFIX="run_8090181"

for leadrun in `ls root/*${PREFIX}*.root | awk -F_ '{print $3}' | awk -F. '{print $1}'`
do
    if [ ! -f root/CosmicStand_${leadrun}.root ]; then
	if [ -s root/output_run_${leadrun}_rb0.root ]; then
	    root -b -l -q merger.C\(${leadrun}\)
	fi
    fi
done
