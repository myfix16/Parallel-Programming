#!/bin/bash
#SBATCH --job-name=mpiomp_16000_10    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=10                   # number of processes = 4
#SBATCH --cpus-per-task=4            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        
mpirun -np=10 build-release-nogui/src/mpiomp 16000 4
mpirun -np=10 build-release-nogui/src/mpiomp 16000 4
mpirun -np=10 build-release-nogui/src/mpiomp 16000 4
mpirun -np=10 build-release-nogui/src/mpiomp 16000 4
mpirun -np=10 build-release-nogui/src/mpiomp 16000 4
