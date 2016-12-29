#include "miscellaneous.h"
#include "geometry.h"

std::string remove_extension(std::string &s) {
    size_t lastdot = s.find_last_of('.');
    if (lastdot == std::string::npos) return s;
    return s.substr(0, lastdot);
}

/*std::istream& operator>> (std::istream& in, Point& v) {
    double d;
    while (in >> d) {
        v.push_back(d);
    }
    return in;
}*/
