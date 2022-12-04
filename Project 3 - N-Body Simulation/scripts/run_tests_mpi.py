from test_profiles import *

cores = [1, 2, 4, 8, 16, 32, 40]


# run mpi jobs
for core in cores:
    for n_body in n_bodies:
        sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=119020038_mpi_{n_body}_{core}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks={core}                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        """
        
        command = f"mpirun -np={core} {build_path}/mpi {n_body} {max_iteration}\n"
        
        run_test(sbatch_config, "mpi.sh", command)