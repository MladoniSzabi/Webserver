#ifndef STRING_CONVERSION_HPP
#define STRING_CONVERSION_HPP

#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>

namespace StringUtils {

template<typename Double = double,
         std::size_t Precision = std::numeric_limits<Double>::digits10,
         typename TimePoint>
inline std::string toString(const TimePoint& timePoint)
{
    auto seconds = Double(timePoint.time_since_epoch().count())
        * TimePoint::period::num / TimePoint::period::den;
    auto const zeconds = std::modf(seconds,&seconds);
    std::time_t tt(seconds);
    std::ostringstream oss;
    auto const tm = std::localtime(&tt);
    if (!tm) throw std::runtime_error(std::strerror(errno));
    oss << std::put_time(tm, "%Y-%b-%d %H:%M:")
        << std::setw(Precision+3) << std::setfill('0')
        << std::fixed << std::setprecision(Precision)
        << tm->tm_sec+zeconds;
    if (!oss) throw std::runtime_error("timepoint-to-string");
    return oss.str();
}

template<typename TimePoint>
TimePoint fromString(const std::string& str)
{
    std::istringstream iss{str};
    std::tm tm{};
    if (!(iss >> std::get_time(&tm, "%Y-%b-%d %H:%M:%S")))
        throw std::invalid_argument("get_time");
    TimePoint timePoint{std::chrono::seconds(std::mktime(&tm))};
    if (iss.eof())
        return timePoint;
    double zz;
    if (iss.peek() != '.' || !(iss >> zz))
        throw std::invalid_argument("decimal");
    using hr_clock = std::chrono::high_resolution_clock;
    std::size_t zeconds = zz * hr_clock::period::den / hr_clock::period::num;
    return timePoint += hr_clock::duration(zeconds);
}

};

#endif