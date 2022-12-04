#!/bin/bash
#SBATCH --job-name=119020038_pthread_5000_64    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task=64            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        
build-release-nogui/src/pthread 5000 100 64
build-release-nogui/src/pthread 5000 100 64
build-release-nogui/src/pthread 5000 100 64
build-release-nogui/src/pthread 5000 100 64
build-release-nogui/src/pthread 5000 100 64
