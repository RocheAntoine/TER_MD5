#!/bin/bash  
#SBATCH --comment "CHPS0711"
#SBATCH -J "MD5_MPI"
#SBATCH --error=logs/job.%J.err  
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=00:30:00
#SBATCH --mem 1024
#SBACTH -n 28
#SBACTH --tasks-per-node=28
#SBATCH -N 1
#SBATCH -c 28
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
module load bullxmpi source /apps/tools/initBatch.sh srun --resv-port /apps/tools/demoJobs/demoBullXmpi