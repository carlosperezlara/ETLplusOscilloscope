#~/bin/sh
suffix=$1
folder=$2

./trc2root \
 -o ${suffix}.root -cmstiming \
 -c1 ${folder}/C1--${suffix}.trc \
 -c2 ${folder}/C2--${suffix}.trc \
 -c3 ${folder}/C3--${suffix}.trc \
 -c4 ${folder}/C4--${suffix}.trc \
 -c5 ${folder}/C5--${suffix}.trc \
 -c6 ${folder}/C6--${suffix}.trc \
 -c7 ${folder}/C7--${suffix}.trc \
 -c8 ${folder}/C8--${suffix}.trc
