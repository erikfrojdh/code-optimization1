#include "Timer.h"
#include "single_photon_hit.h"
#include "histogram.h"

#include "TFile.h"

#include <iomanip>
#include <iostream>
#include <stdio.h>


int main() {
    constexpr int n_items = 1'000'000'000;
    std::cout << "itemsize on disk is: "
              << 4 * sizeof(int) + 3 * sizeof(int16_t) + sizeof(int32_t) + 4 * sizeof(double) + sizeof(quadrant)
              << " + " << 9 * sizeof(int32_t) << "\n";
    std::cout << "Loading " << n_items << " clusters from disk\n";

    auto th2 = make_hist("name", "title");

    FILE *pFile = fopen("/home/l_frojdh/tmp/Module77_ST_Pb75keV_-wCM-dHC.clust", "rb");
    if (pFile) {
        sls::Timer timer;
        auto hit = single_photon_hit();
        for (int i = 0; i != n_items; ++i) {
            hit.read(pFile);
            th2->Fill(hit.x*256+hit.y, hit.tot);
        }
        auto duration = timer.elapsed_s();
        auto throughput = static_cast<double>(n_items) * 98. / duration / (1024. * 1024.);
        printf("Duration: %.2f s processed %.2f MB/s\n", duration, throughput);
        
        fclose(pFile);
    }else{
        std::cout << "ERROR: Could not open file\n";
    }

    //Write out data for verification
    auto projy = th2->ProjectionY();
    auto pixel = th2->ProjectionY("pixel5000", 5000, 5001);
    TFile *f = new TFile("test0.root", "RECREATE");
    projy->Write();
    pixel->Write();
    f->Close();
}