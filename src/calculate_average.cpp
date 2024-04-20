#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>

struct Number
{
    std::int64_t digits;
};

class Measurements
{
    std::int64_t _count = 0;
    std::int64_t _min = 0;
    std::int64_t _max = 0;
    std::int64_t _sum = 0;

public:
    void record(std::int64_t measurement)
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
            if (measurement < _min)
                _min = measurement;
            if (measurement > _max)
                _max = measurement;
            _sum += measurement;
            _count++;
        }
    }

    Number mean() const
    {
        auto digits = (_sum * 10) / _count;

        if (std::abs(digits % 10) >= 5)
        {
            return {(digits / 10) + (digits < 0 ? -1 : 1)};
        }

        return {digits / 10};
    }

    Number min() const
    {
        return {_min};
    }

    Number max() const
    {
        return {_max};
    }

    auto sum() const -> std::int64_t
    {
        return _sum;
    }
    
    auto count() const -> std::int64_t
    {
        return _count;
    }
};

// real    2m11.421s
// user    1m58.426s
// sys     0m8.36
static inline void readStations(std::ifstream &file, std::unordered_map<std::string, Measurements> &stations);

static inline std::ostream &operator<<(std::ostream &os, const Number &number)
{
    auto wholeDigits = number.digits / 10;

    if (wholeDigits == 0 && number.digits < 0) {
        os << '-';
    }

    return os << (number.digits / 10) << '.' << std::abs(number.digits % 10);
}

int main()
{
    std::ifstream file{"measurements.txt"};
    std::unordered_map<std::string, Measurements> stations;

    // SLOW!!!
    readStations(file, stations);

    // Sort the keys
    std::vector<std::string> keys;
    keys.reserve(stations.size());
    for (const auto &data : stations)
    {
        keys.push_back(data.first);
    }
    std::sort(keys.begin(), keys.end());

    // Print the results
    std::cout << '{';
    std::size_t i = 0;

    // Print each station summary using the format: <station>=<min>/<mean>/<max>
    for (const auto &key : keys)
    {
        const auto &data = stations.at(key);
        std::cout << key << '='
                  << std::fixed << std::setprecision(1)
                  << data.min() << '/'
                  << data.mean() << '/'
                  << data.max();

        if (i + 1 < stations.size())
        {
            std::cout << ", ";
        }

        i++;
    }

    std::cout << '}';

    return 0;
}

void readStations(std::ifstream &file, std::unordered_map<std::string, Measurements> &stations)
{
    enum ParserStatus
    {
        StationName,
        Measurement,
        PositiveMeasurement,
        NegativeMeasurement
    };

    // Read each line using the format: <station>;<measurement>\n
    std::string station{};
    char c{};
    std::int64_t measurement{};
    ParserStatus status{ParserStatus::StationName};

    // Read each character in the file
    while (file.get(c))
    {
        switch (status)
        {
        case ParserStatus::StationName:
        {
            switch (c)
            {
            case ';':
                status = ParserStatus::Measurement;
                break;
            default:
                station += c;
            }
        }
        break;
        case ParserStatus::Measurement:
        {
            switch (c)
            {
            case '-':
                status = ParserStatus::NegativeMeasurement;
                measurement = 0;
                break;
            default:
                status = ParserStatus::PositiveMeasurement;
                measurement = static_cast<std::int64_t>(c - '0');
            }
        }
        break;
        case ParserStatus::PositiveMeasurement:
        {
            switch (c)
            {
            case '\n':
                status = ParserStatus::StationName;
                stations[station].record(measurement);
                station.clear();
                break;
            case '.':
                break;
            default:
                measurement = measurement * 10 + static_cast<std::int64_t>(c - '0');
            }
        }
        break;
        case ParserStatus::NegativeMeasurement:
        {
            switch (c)
            {
            case '\n':
                status = ParserStatus::StationName;
                stations[station].record(measurement);
                station.clear();
                break;
            case '.':
                break;
            default:
                measurement = measurement * 10 - static_cast<std::int64_t>(c - '0');
            }
        }
        }
    }
}