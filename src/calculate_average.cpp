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
    inline auto record(std::int64_t measurement) -> void
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

    inline auto mean() const -> Number
    {
        auto digits = (_sum * 10) / _count;

        if (std::abs(digits % 10) >= 5)
        {
            return {(digits / 10) + (digits < 0 ? -1 : 1)};
        }

        return {digits / 10};
    }

    inline auto min() const -> Number
    {
        return {_min};
    }

    inline auto max() const -> Number
    {
        return {_max};
    }

    inline auto sum() const -> std::int64_t
    {
        return _sum;
    }

    inline auto count() const -> std::int64_t
    {
        return _count;
    }
};

static inline auto readStations(std::ifstream &file, std::unordered_map<std::string, Measurements> &stations) -> void
{
    enum ParserStatus
    {
        StationName,
        Measurement,
        PositiveMeasurement,
        NegativeMeasurement
    };

    // Read each line using the format: <station>;<measurement>\n
    auto station = std::string{};
    auto c = char{};
    auto measurement = std::int64_t{0};
    auto status = ParserStatus::StationName;

    // Read chunks of memory
    constexpr auto chunkSize = std::streamsize{4096};
    auto chunk = std::array<char, chunkSize>{};

    // Read each character in the file
    while (file.read(chunk.data(), chunkSize))
    {
        for (auto i = 0; i < file.gcount(); i++)
        {
            c = chunk[i];
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
}

static inline auto operator<<(std::ostream &os, const Number &number) -> std::ostream &
{
    auto wholeDigits = number.digits / 10;
    auto decimalDigits = std::abs(number.digits % 10);

    if (wholeDigits == 0 && number.digits < 0)
    {
        os << '-';
    }

    return os << wholeDigits << '.' << decimalDigits;
}

static inline auto operator<<(std::ostream &os, const Measurements &measurements) -> std::ostream &
{
    return os << measurements.min() << '/'
              << measurements.mean() << '/'
              << measurements.max();
}

auto main() -> int
{
    auto file = std::ifstream{"measurements.txt", std::ios::binary};
    auto stations = std::unordered_map<std::string, Measurements>{};

    // TODO: Read the file using threads
    readStations(file, stations);

    // Copy and sort the station names
    auto keys = std::vector<std::string>{stations.size()};
    std::transform(stations.cbegin(),
                   stations.cend(),
                   keys.begin(),
                   [](const auto &station)
                   { return station.first; });
    std::sort(keys.begin(), keys.end());

    // Print the results
    std::cout << '{';

    // Print each station summary using the format: <station>=<min>/<mean>/<max>
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