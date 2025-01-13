for X in `seq 0 999`: ### 1000 bunches
do
    RUN=`cat runningIndex.txt`
    date
    echo "PREPARING KCU ${X} out of 1000 >>  RUN ${RUN}"
    cd ~/ETROC2_Test_Stand/module_test_sw/
    echo "STOP" > lockfile.conf
    source configone.sh 18 0 > ETROC_output/logger_${RUN}.txt
    source quickone.sh ${RUN} >> ETROC_output/logger_${RUN}.txt &
    cd -
    echo "START" > /home/daq/ETROC2_Test_Stand/module_test_sw/lockfile.conf
    for Y in `seq 0 29`: ### thirty runs ~2h
    do
	echo "OSC ${Y} out of 29"
	time python3 cosmicstand.py
    done
    echo "STOP" > /home/daq/ETROC2_Test_Stand/module_test_sw/lockfile.conf
    echo " END OF ETROC ACCUMULATOR. TAKING A 1m BREAK FOR SYSTEM TO CATCH UP" >> ETROC_output/logger_${RUN}.txt
    sleep 60
    echo " END OF ETROC ACCUMULATOR. DONE" >> ETROC_output/logger_${RUN}.txt
done
