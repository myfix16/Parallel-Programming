import os
import re

from test_profiles import test_repeat_num

out_dir = "outputs"
sta_file = "stats.csv"
sta_path = os.path.join(out_dir, sta_file)
num_lines_per_run = 6

# open statistics file
sta = open(sta_path, "w+")
sta.write("type, cores, size, time (s)\n")

for file in os.listdir(out_dir):
    with open(os.path.join(out_dir, file), "r") as f:
        if (file == sta_file): continue
        
        content = f.readlines()
        
        times = [float(re.match(r"Converge after \d+ iterations, elapsed time: (.+), average computation time: (.+)", content[i]).groups()[0]) for i in 
                [0 + num_lines_per_run * j for j in range(test_repeat_num)]]
        time_ = sum(times) / test_repeat_num
        
        type_ = re.match(r"Assignment 4: Heat Distribution (.+) Implementation", content[3]).groups()[0]
        num = int(re.match(r"Problem Size: (\d+)", content[4]).groups()[0])
        num_cores = re.match(r"Number of Cores: (\d+)", content[5]).groups()[0]

        # write to csv
        sta.write(f"{type_}, {num_cores}, {num}, {time_}\n")

sta.close()