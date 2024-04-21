# One Billion Row Challenge (C++ 17)
Configure the build files using the command below.
```bash
cmake .
```
Build the binaries using the command below.

```bash
cmake --build .
```

To generate the rows, use the command below.
```bash
./create_measurements 1000000000 
```

Run the baseline algorithm using the command below.
```bash
time ./calculate_average_baseline
```

Run the fastest algorithm using the command below.
```bash
time ./calculate_average
```