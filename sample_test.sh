./create_measurements 10000
./calculate_average_baseline > reference.txt
./calculate_average > out.txt
./validate.sh out.txt