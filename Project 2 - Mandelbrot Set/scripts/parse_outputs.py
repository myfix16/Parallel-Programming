import os
import re

from test_profiles import test_repeat_num

out_dir = "outputs"
sta_file = "stats.csv"
sta_path = os.path.join(out_dir, sta_file)
num_lines_per_run = 7

# open statistics file
sta = open(sta_path, "w+")
sta.write("type, cores, size, time (s), speed (pixels/s)\n")

for file in os.listdir(out_dir):
    with open(os.path.join(out_dir, file), "r") as f:
        if (file == sta_file): continue
        
        content = f.readlines()
        
        num = int(re.match(r"Problem Size: (\d+) \* \d+, \d+", content[4]).groups()[0])
        times = [float(re.match(r"Run Time: (.+) seconds", content[i]).groups()[0]) for i in 
                [3 + num_lines_per_run * j for j in range(test_repeat_num)]]
        time_ = sum(times) / test_repeat_num
        speed = num ** 2 / time_
        
        type_ = re.match(r"Assignment 2: (.+)", content[2]).groups()[0]
        # sequential
        if type_ == "Sequential":
            sta.write(f"Sequential, 1, {num}, {time_}, {speed}\n")
        # pthread
        elif type_ == "Pthread":
            num_cores = re.match(r"Thread Number: (\d+)", content[6]).groups()[0]
            sta.write(f"Pthread, {num_cores}, {num}, {time_}, {speed}\n")
        # mpi
        else:
            num_cores = re.match(r"Process Number: (\d+)", content[6]).groups()[0]
            sta.write(f"MPI, {num_cores}, {num}, {time_}, {speed}\n")

sta.close()