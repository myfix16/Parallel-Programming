from test_profiles import *


# run sequential jobs
for size in sizes:
    sbatch_config: str = \
    f"""\
#!/bin/bash
#SBATCH --job-name=seq_{size}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Debug          # Partition name: Project or Debug (Debug is default)
    """
    
    command = f"{build_path}/sequential {size}\n"
    
    run_test(sbatch_config, "sequential.sh", command)
        