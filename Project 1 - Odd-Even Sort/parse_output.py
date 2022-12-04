import os
import re

out_dir = "outputs"
sta_file = "stats.csv"
sta_path = os.path.join(out_dir, sta_file)

# open statistics file
sta = open(sta_path, "w+")
sta.write("type, cores, array size, time (s)\n")

for file in os.listdir(out_dir):
    with open(os.path.join(out_dir, file), "r") as f:
        if (file == sta_file): continue
        
        content = f.readlines()
        num = re.match(r"actual number of elements:(\d+)", content[1]).groups()[0]
        time_ = re.match(r"Run Time: (.+) seconds", content[5]).groups()[0]
        
        # sequential
        if (content[0].strip() == "seq"):
            sta.write(f"seq, 1, {num}, {time_}\n")
        # parallel
        else:
            num_cores = re.match(r"Process Number: (\d+)", content[7]).groups()[0]
            sta.write(f"par, {num_cores}, {num}, {time_}\n")

sta.close()