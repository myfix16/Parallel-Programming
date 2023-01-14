from test_profiles import *

cores = [1, 2, 4, 8, 16, 32, 40]


# run pthread jobs
for core in cores:
    for size in sizes:
        sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=pthread_{size}_{core}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task={core}            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        """
        
        command = f"{build_path}/pthread {size} {core}\n"
        
        run_test(sbatch_config, "pthread.sh", command)