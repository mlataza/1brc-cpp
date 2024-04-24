# One Billion Row Challenge (C++ 17)
Built using CMake. Developed in Ubuntu WSL2.

The executables are built in the `build` folder. Run the executables within the `build` folder.

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