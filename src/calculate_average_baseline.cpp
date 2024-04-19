#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <cmath>

struct Measurements
{
    int count = 0;
    double min = 0.0;
    double max = 0.0;
    double sum = 0.0;

    void record(double measurement)
    {
        if (count == 0) {
            min = measurement;
            max = measurement;
            sum = measurement;
            count = 1;
        } else {
            if (measurement < min) min = measurement;
            if (measurement > max) max = measurement;
            sum += measurement;
            count++;
        }
    }

    double mean() const
    {
        return std::round(sum * 10.0 / count) / 10.0;
    }
};

int main()
{
    std::ifstream file{"measurements.txt"};
    std::string station, measurement;
    std::map<std::string, Measurements> stations;

    // Read each line using the format: <station>;<measurement>\n
    while (std::getline(file, station, ';') && std::getline(file, measurement))
    {
        stations[station].record(std::stod(measurement));
    }

    // Print the results 
    std::cout << '{';
    std::size_t i = 0;

    // Print each station summary using the format: <station>=<min>/<mean>/<max>
    for (auto data : stations) {
        std::cout << data.first << '='
                << std::fixed << std::setprecision(1)
                << data.second.min << '/'
                << data.second.mean() << '/'
                << data.second.max;

        if (i + 1 < stations.size()) {
            std::cout << ", ";
        }

        i++;
    }

    std::cout << '}';

    return 0;
}