#!/bin/sh
# embedded options to qsub - start with #PBS
# -- Name of the job ---
#PBS -N Java_Search
# -- specify queue --
#PBS -q hpc
# -- estimated wall clock time (execution time): hh:mm:ss --
#PBS -l walltime=00:01:00
# -- number of processors/cores/nodes --
#PBS -l nodes=1:ppn=10
# -- user email address --
##PBS -M sXXXXXX@student.dtu.dk
# -- mail notification --
##PBS -m abe
# -- run in the current working (submission) directory --
if test X$PBS_ENVIRONMENT = XPBS_BATCH && test X$PBS_O_QUEUE != Xapp
then 
  cd $PBS_O_WORKDIR
fi
# here follow the commands you want to execute
echo $PWD

# Set file and pattern
export TEXTFILE=shake.txt
export PATTERN="Something"
# Choose either fixed number of threads or use number of cores
export NTHREADS=4
# export  NTHREADS=$PBS_NUM_PPN

# Create empty file (to be appended to)
cat </dev/null >java.log

export NTASKS_LIST="1 2 4 8 10 12 16 20 40 60 80 100"

# Run search for varying number of tasks
for NTASKS in $NTASKS_LIST
do
  java Search $TEXTFILE $PATTERN $NTASKS $NTHREADS >> java.log
done
