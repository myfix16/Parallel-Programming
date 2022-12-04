import os

from test_profiles import *


# run sequential jobs
for x, max_iter in dims:
    sbatch_config: str = \
    f"""\
#!/bin/bash
#SBATCH --job-name=119020038_seq_{x}_{max_iter}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Debug          # Partition name: Project or Debug (Debug is default)
    """
    
    seq_command = f"cmake-build-release/sequential {x} {x} {max_iter}\n"
    
    with open("scripts/job.sh", "w+") as f:
        f.write(sbatch_config)
        f.write("\n")
        for _ in range(test_repeat_num):
            f.write(seq_command)
        
    os.system("sbatch scripts/job.sh")
        