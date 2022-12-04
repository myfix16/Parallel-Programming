import os

from test_profiles import *

cores = range(1, 21)


# run mpi jobs
for core in cores:
    for x, max_iter in dims:
        sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=119020038_mpi_{x}_{max_iter}_{core}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks={core}                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        """
        
        seq_command = f"mpirun -np={core} cmake-build-release/mpi {x} {x} {max_iter}\n"
        
        with open("scripts/job.sh", "w+") as f:
            f.write(sbatch_config)
            f.write("\n")
            for _ in range(test_repeat_num):
                f.write(seq_command)
            
        os.system("sbatch scripts/job.sh")