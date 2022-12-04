from test_profiles import *


# run cuda jobs
for n_body in n_bodies:
    sbatch_config: str = \
    f"""\
#!/bin/bash
#SBATCH --job-name=119020038_cuda_{n_body}    # Job name
#SBATCH --gres=gpu:1                ## Number of GPUs required for job execution.
#SBATCH --partition=Project           ## the partitions to run in (Debug or Project)
#SBATCH --ntasks=1                  ## number of tasks (analyses) to run
#SBATCH --gpus-per-task=1           ## number of gpus per task
#SBATCH --time=0-00:10:00           ## time for analysis (day-hour:min:sec)
    """
    
    command = f"{build_path}/cuda {n_body} {max_iteration}\n"
    
    run_test(sbatch_config, "cuda.sh", command)
        