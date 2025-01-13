CUR_DIR=`pwd`
RUN_PREFIX="80901810"

#C1--Cosmics_1018100000084.trc
for lecroyfile in `ls trc/C1--Cosmics_${RUN_PREFIX}*.trc | awk -F. '{print $1}' | awk -F- '{print $3}'`
do
    ##### LECROY DATA
    if [ ! -f ${CUR_DIR}/root/${lecroyfile}.root ]; then
	echo "processing lecroy data for ${lecroyfile}"
	cd ~/trc2root
	source trc2root.sh ${lecroyfile} ${CUR_DIR}/trc
	mv ${lecroyfile}.root ${CUR_DIR}/root
	cd -
    fi
done


#output_run_1018100000084_rb0.dat
for etlfile in `ls dat/*${RUN_PREFIX}*_rb0.dat | awk -F/ '{print $2}' | awk -F. '{print $1}'`
do
    ##### ETL DATA
    if [ -s ${CUR_DIR}/dat/${etlfile}.dat ]; then
	if [ ! -f ${CUR_DIR}/dat/${etlfile}.root ]; then
	    echo "processing ETL data for ${etlfile}"
	    cd ~/module_test_sw
	    source dat2root.sh ${etlfile} ${CUR_DIR}/dat
	    mv ${etlfile}.root ${CUR_DIR}/root
	    cd -
	fi
    else
	echo " ${etlfile}.dat is empty!"
    fi
done
