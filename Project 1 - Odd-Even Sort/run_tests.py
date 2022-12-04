import os

num_cores = [1, 2, 4, 8 ,16]
small_sizes = [100, 200, 300, 400, 500]
medium_sizes = [10000, 20000, 30000, 40000, 50000]
large_sizes = [100000, 200000, 300000, 400000, 500000]
arr_sizes = [*small_sizes, *medium_sizes, *large_sizes]
run_seq = False
run_par = True

# ensure all input files are generated
for size in arr_sizes:
    filepath = f"test_data/{size}.in"
    if (not os.path.exists(filepath)):
        os.system(f"build/gen {size} {filepath}")
        
# run sequential jobs
if run_seq:
    for size in arr_sizes:
        sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=119020038_seq_{size}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks=1                   # number of processes = 4
#SBATCH --cpus-per-task=1            # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:60:00              # Time limit hrs:min:sec
#SBATCH --partition=Debug          # Partition name: Project or Debug (Debug is default)
        """
        
        seq_command = f"build/ssort {size} test_data/{size}.in"
        
        with open("test_profiles/job.sh", "w+") as f:
            f.write(sbatch_config)
            f.write("\n")
            f.write(seq_command)
            
        os.system("sbatch test_profiles/job.sh")
        
# run parallel jobs
if run_par:
    for size in arr_sizes:
        for num_core in num_cores:
            sbatch_config: str = \
        f"""\
#!/bin/bash
#SBATCH --job-name=119020038_par_{size}_{num_core}    # Job name
#SBATCH --nodes=1                    # Run all processes on a single node	
#SBATCH --ntasks={num_core}                   # number of processes = 4
#SBATCH --cpus-per-task=1           # Number of CPU cores per process
#SBATCH --mem=1000mb                  # Total memory limit
#SBATCH --time=00:10:00              # Time limit hrs:min:sec
#SBATCH --partition=Project          # Partition name: Project or Debug (Debug is default)
        """
        
            seq_command = f"mpirun -np={num_core} build/psort {size} test_data/{size}.in"
            
            with open("test_profiles/job.sh", "w+") as f:
                f.write(sbatch_config)
                f.write("\n")
                f.write(seq_command)
                
            os.system("sbatch test_profiles/job.sh")
