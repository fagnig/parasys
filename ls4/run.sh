#!/bin/sh
# embedded options to qsub - start with #PBS
# -- Name of the job ---
#PBS -N Hello_MPI
# -- specify queue --
#PBS -q hpc
# -- estimated wall clock time (execution time): hh:mm:ss --
#PBS -l walltime=00:01:00
# -- number of processors/cores/nodes --
#PBS -l nodes=2:ppn=4
# -- user email address --
##PBS -M sXXXXXX@student.dtu.dk
# -- mail notification --
#PBS -m abe
# -- run in the current working (submission) directory --
if test X$PBS_ENVIRONMENT = XPBS_BATCH; then cd $PBS_O_WORKDIR; fi
# here follow the commands you want to execute

module load mpi

mpirun ./hello_mpi > mpi.log
