#pragma once

#include "TH2D.h"
#include "TH2F.h"
#include "TH2S.h"
#include "TH2C.h"
#include "TFile.h"
#include <string>


constexpr int npixels = 256 * 256;
constexpr double xmin = 0;
constexpr double xmax = 256. * 256.;
constexpr int nbinsy = 180;
constexpr double ymax = 3600;
constexpr double ymin = 0;


auto make_hist(const std::string& name, const std::string& title){
    return new TH2S(name.c_str(), title.c_str(), npixels, xmin, xmax, nbinsy, ymin, ymax);
}


void save_hist(const std::string& fname, TH2* th2){
    auto projy = th2->ProjectionY();
    auto pixel = th2->ProjectionY("pixel5000", 5000, 5001);
    TFile *f = new TFile(fname.c_str(), "RECREATE");
    projy->Write();
    pixel->Write();
    th2->Write();
    f->Close();

}