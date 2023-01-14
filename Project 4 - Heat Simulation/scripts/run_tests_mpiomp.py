from test_profiles import *

cores = [1, 2, 4, 8, 10]
omp_cores = 4

# run mpiomp jobs
for core in cores:
    for size in sizes:
        sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=mpiomp_{size}_{core}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks={core}                   # number of processes = 4
#SBATCH --cpus-per-task={omp_cores}            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        """
        
        command = f"mpirun -np={core} {build_path}/mpiomp {size} {omp_cores}\n"
        
        run_test(sbatch_config, "mpiomp.sh", command)