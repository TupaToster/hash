file = "crc32Hash.hist"

with open(file, "r") as f:
    lines = f.readlines()

numbers = [int(line.strip()) for line in lines]

cnt = 0
for i in numbers:
    if i != 0: cnt+=1

cnt = sum (numbers) / cnt

print("The average is:", cnt)