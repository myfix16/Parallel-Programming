from test_profiles import *


# run cuda jobs
for size in sizes:
    sbatch_config: str = \
    f"""\
#!/bin/bash
#SBATCH --job-name=cuda_{size}    # Job name
#SBATCH --gres=gpu:1                ## Number of GPUs required for job execution.
#SBATCH --partition=Project           ## the partitions to run in (Debug or Project)
#SBATCH --ntasks=1                  ## number of tasks (analyses) to run
#SBATCH --gpus-per-task=1           ## number of gpus per task
#SBATCH --time=0-00:10:00           ## time for analysis (day-hour:min:sec)
    """
    
    command = f"{build_path}/cuda {size}\n"
    
    run_test(sbatch_config, "cuda.sh", command)
        