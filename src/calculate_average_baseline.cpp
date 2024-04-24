#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <cmath>

constexpr auto roundToPositive(double n) noexcept -> double
{
    return std::floor(n + 0.5);
}

class Measurements
{
    int _count = 0;
    double _min = 0.0;
    double _max = 0.0;
    double _sum = 0.0;

public:
    constexpr auto record(double measurement) noexcept -> void
    {
        if (_count == 0)
        {
            _min = measurement;
            _max = measurement;
            _sum = measurement;
            _count = 1;
        }
        else
        {
            _min = std::min(_min, measurement);
            _max = std::max(_max, measurement);
            _sum += measurement;
            _count++;
        }
    }

    constexpr auto mean() const noexcept -> double
    {
        auto average = roundToPositive(_sum * 10.0) / 10.0 / _count;
        return roundToPositive(average * 10.0) / 10.0;
    }

    constexpr auto min() const noexcept -> double
    {
        return roundToPositive(_min * 10.0) / 10.0;
    }

    constexpr auto max() const noexcept -> double
    {
        return roundToPositive(_max * 10.0) / 10.0;
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
    for (auto data : stations)
    {
        std::cout << data.first << '='
                  << std::fixed << std::setprecision(1)
                  << data.second.min() << '/'
                  << data.second.mean() << '/'
                  << data.second.max();

        if (i + 1 < stations.size())
        {
            std::cout << ", ";
        }

        i++;
    }

    std::cout << '}';

    return 0;
}