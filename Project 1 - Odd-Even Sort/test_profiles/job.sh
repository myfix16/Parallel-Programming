#!/bin/bash
#SBATCH --job-name=119020038_par_500000_16    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=16                   # number of processes = 4
#SBATCH --cpus-per-task=1           # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        
mpirun -np=16 build/psort 500000 test_data/500000.in