from data import LichessDataset

import time
start = time.time()
ld = LichessDataset(["/home/adam/Downloads/lichess_elite/lichess_elite_2020-06.pgn"])
end = time.time()

print(f"dur: {end - start}")

for i in range(100):
  print(ld[i]["y_policy"])
  print(ld[i]["y_value"])
