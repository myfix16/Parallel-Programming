#!/bin/bash
#SBATCH --job-name=119020038_mpi_5000_40    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=40                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        
mpirun -np=40 build-release-nogui/src/mpi 5000 100
mpirun -np=40 build-release-nogui/src/mpi 5000 100
mpirun -np=40 build-release-nogui/src/mpi 5000 100
mpirun -np=40 build-release-nogui/src/mpi 5000 100
mpirun -np=40 build-release-nogui/src/mpi 5000 100
