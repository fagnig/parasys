#!/bin/sh
# embedded options to qsub - start with #PBS
# -- Name of the job ---
#PBS -N Hello_OMP
# -- specify queue --
#PBS -q hpc
# -- estimated wall clock time (execution time): hh:mm:ss --
#PBS -l walltime=00:01:00
# -- number of processors/cores/nodes --
#PBS -l nodes=1:ppn=6
# -- user email address --
##PBS -M s175179@student.dtu.dk
# -- mail notification --
##PBS -m abe
# -- run in the current working (submission) directory --
if test X$PBS_ENVIRONMENT = XPBS_BATCH; then cd $PBS_O_WORKDIR; fi
# here follow the commands you want to execute
echo $PWD

# set OMP_NUM_THREADS _and_ export!
OMP_NUM_THREADS=$PBS_NUM_PPN
export  OMP_NUM_THREADS

./omp_hello > omp.log

