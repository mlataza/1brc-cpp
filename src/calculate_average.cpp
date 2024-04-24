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
#include <array>
#include <filesystem>

class Measurements
{
    std::int64_t _count = 0;
    std::int64_t _min = 0;
    std::int64_t _max = 0;
    std::int64_t _sum = 0;

public:
    constexpr auto record(std::int64_t measurement) noexcept
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

    constexpr auto roundToPositive(double n) const noexcept
    {
        // std::round uses "round to zero" but Math.round in Java uses "round to positive"
        return std::floor(n + 0.5);
    }

    constexpr auto round(double n) const noexcept
    {
        // Round to nearest tenths place
        return roundToPositive(n * 10.0) / 10.0;
    }

    constexpr auto mean() const noexcept
    {
        return round(static_cast<double>(_sum) / 10.0 / _count);
    }

    constexpr auto min() const noexcept
    {
        return round(static_cast<double>(_min) / 10.0);
    }

    constexpr auto max() const noexcept
    {
        return round(static_cast<double>(_max) / 10.0);
    }

    constexpr auto merge(const Measurements &measurements) noexcept
    {
        if (_count == 0)
        {
            *this = measurements;
        }
        else
        {
            _min = std::min(_min, measurements._min);
            _max = std::max(_max, measurements._max);
            _sum += measurements._sum;
            _count += measurements._count;
        }
    }

    friend inline auto operator<<(std::ostream &os, const Measurements &measurements) noexcept -> std::ostream &
    {
        return os << measurements.min() << '/'
                  << measurements.mean() << '/'
                  << measurements.max();
    }
};

class Parser
{
    enum ParserStatus
    {
        StationName,
        Measurement,
        PositiveMeasurement,
        NegativeMeasurement
    };

    std::string _station;
    std::int64_t _measurement = 0;
    ParserStatus _status = ParserStatus::StationName;

public:
    template <typename InputIterator, typename Map>
    auto operator()(InputIterator begin, InputIterator end, Map &stations) noexcept
    {
        for (auto it = begin; it != end; it++)
        {
            char c = *it;

            switch (_status)
            {
            case ParserStatus::StationName:
            {
                switch (c)
                {
                case ';':
                    _status = ParserStatus::Measurement;
                    break;
                default:
                    _station += c;
                }
            }
            break;
            case ParserStatus::Measurement:
            {
                switch (c)
                {
                case '-':
                    _status = ParserStatus::NegativeMeasurement;
                    _measurement = 0;
                    break;
                default:
                    _status = ParserStatus::PositiveMeasurement;
                    _measurement = static_cast<std::int64_t>(c - '0');
                }
            }
            break;
            case ParserStatus::PositiveMeasurement:
            {
                switch (c)
                {
                case '\n':
                    _status = ParserStatus::StationName;
                    stations[_station].record(_measurement);
                    _station.clear();
                    break;
                case '.':
                    break;
                default:
                    _measurement = _measurement * 10 + static_cast<std::int64_t>(c - '0');
                }
            }
            break;
            case ParserStatus::NegativeMeasurement:
            {
                switch (c)
                {
                case '\n':
                    _status = ParserStatus::StationName;
                    stations[_station].record(_measurement);
                    _station.clear();
                    break;
                case '.':
                    break;
                default:
                    _measurement = _measurement * 10 - static_cast<std::int64_t>(c - '0');
                }
            }
            }
        }
    }
};

using MapType = std::unordered_map<std::string, Measurements>;

constexpr auto chunkSize = 1 << 12;

auto process(int index, int numberOfThreads, MapType &stations) noexcept
{
    // Compute the chunk start and size
    auto fileSize = std::filesystem::file_size("measurements.txt");
    auto partSize = (fileSize + numberOfThreads - 1) / numberOfThreads; // ceiling
    auto partStart = partSize * index;
    auto partEnd = std::min(partStart + partSize, fileSize);

    // Adjust the part start and end pointers
    auto file = std::ifstream{"measurements.txt", std::ios::binary};
    if (index + 1 < numberOfThreads)
    {
        // Adjust partEnd to align after '\n' character
        for (char c; file.seekg(partEnd - 1), file.read(&c, 1), c != '\n'; partEnd++)
        {
        }
    }

    if (0 < index)
    {
        // Adjust partStart to align after '\n' character
        for (char c; file.seekg(partStart - 1), file.read(&c, 1), c != '\n'; partStart++)
        {
        }
    }

    // Read chunks of memory
    auto parser = Parser{};
    auto chunk = std::array<char, chunkSize>{};

    // Move file pointer to partStart
    file.seekg(partStart);

    // Read each line using the format: <station>;<measurement>\n
    for (auto current = partStart; current < partEnd; current += chunkSize)
    {
        auto size = std::min(static_cast<std::uintmax_t>(chunkSize), partEnd - current);
        file.read(chunk.data(), size);
        parser(chunk.cbegin(), chunk.cbegin() + size, stations);
    }
}

int main()
{
    auto stations = MapType{};

    // Read the file using threads
    auto threads = std::vector<std::thread>{};
    auto numberOfThreads = static_cast<int>(std::thread::hardware_concurrency());
    auto stationMaps = std::vector<MapType>{static_cast<std::size_t>(numberOfThreads)};
    for (auto i = 0; i < numberOfThreads; i++)
    {
        threads.push_back(std::thread{process, i, numberOfThreads, std::ref(stationMaps.at(i))});
    }

    // Wait for the threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Merge stations
    for (const auto &map : stationMaps)
    {
        for (const auto &[key, measurements] : map)
        {
            stations[key].merge(measurements);
        }
    }

    // Copy and sort the station names
    auto keys = std::vector<std::string>{};
    for (const auto &pair : stations)
    {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());

    // Print each station summary using the format: <station>=<min>/<mean>/<max>
    std::cout << '{' << std::fixed << std::setprecision(1);
    for (auto it = keys.cbegin(); it != keys.cend(); it++)
    {
        const auto &key = *it;
        std::cout << key << '=' << stations.at(key);

        if (it + 1 != keys.cend())
        {
            std::cout << ", ";
        }
    }
    std::cout << '}';

    return 0;
}