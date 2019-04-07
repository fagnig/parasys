#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
echo -e "--- Cleaning up previous files ---"
rm -f filter
rm -f images/*-processed*
echo -e "--- Compiling... ---\n"
timeserial=0
timeopenmp=0
gcc -fopenmp -std=c99 filter.c writeBMP.c readBMP.c -o filter
if [ $? -ne 0 ];
then
	echo -e "${RED}FAILED"
	echo -e "Compile failed."
	else
		for filename in images/*.bmp; do
			proc=$(echo $filename | grep processed)
			if [[ -z $proc ]];
			then
				echo -e "${NC}--- Testing program on :" $filename "---"
				out="$(./filter $filename)"
				err=$(echo "$out" | grep ERROR)
				timeser=$(echo "$out" | grep 'Elapsed time, serial')
				timeomp=$(echo "$out" | grep 'Elapsed time, using omp')
				wrote=$(echo "$out" | grep Wrote)
				echo -e "$wrote"
				if [[ -z $timeser ]]; then
					echo -e "${RED}Runtime error exception, run the program with ./filter <filename>"
					echo -e "${RED}FAILED"
				elif [[ -z $err ]]; then
					t1=$(echo "$timeser" | cut -d= -f2)
					t2=$(echo "$timeomp" | cut -d= -f2)
					timeserial=$(echo $t1 + $timeserial | bc) 
					timeopenmp=$(echo $t2 + $timeopenmp | bc) 
					echo -e "${GREEN}PASSED"
					echo -e "$timeser"
					echo -e "$timeomp"
				else
					echo -e "${RED}FAILED"
					echo -e "$err"
				fi
			else
				:
			fi
			echo -e ""
		done
fi
echo -e "OMP_NUM_THREADS: $OMP_NUM_THREADS"
echo -e "Total serial time: $timeserial"
echo -e "Total openMP time: $timeopenmp"
echo -e "${NC}Test complete."