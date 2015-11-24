#!/bin/bash
#SBATCH --job-name=<NAME>
#SBATCH --ntasks=<NTASKS>
#SBATCH --ntasks-per-node=<NTASKSPERNODE>
#SBATCH --cpus-per-task=<CPUSPERTASK>
#SBATCH --output=<OUTFILE>
#SBATCH --time=00:30:00
#SBATCH --partition=primary
#SBATCH --nodelist=opcode3
#SBATCH --gres=gpu:8

########################################################

set -x
export OMP_NUM_THREADS=<CPUSPERTASK>
export MV2_USE_CUDA=1
export MV2_ENABLE_AFFINITY=0
export CUDA_VA_RESERVATION_CHUNK_SIZE=4294967296

<CMD>

########################################################
