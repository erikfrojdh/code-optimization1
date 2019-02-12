#include "TH2D.h"
#include <string>


constexpr int npixels = 256 * 256;
constexpr double xmin = 0;
constexpr double xmax = 256. * 256.;
constexpr int nbinsy = 180;
constexpr double ymax = 3600;
constexpr double ymin = 0;


TH2D* make_hist(const std::string& name, const std::string& title){
    return new TH2D(name.c_str(), title.c_str(), npixels, xmin, xmax, nbinsy, ymin, ymax);
}
