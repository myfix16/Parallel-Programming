#!/bin/bash
#SBATCH --job-name=119020038_pthread_3200_100_20    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task=20            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        
cmake-build-release/pthread 3200 3200 100 20
cmake-build-release/pthread 3200 3200 100 20
cmake-build-release/pthread 3200 3200 100 20
cmake-build-release/pthread 3200 3200 100 20
cmake-build-release/pthread 3200 3200 100 20
