#include "SinglePhotonHit.h"
#include "Timer.h"
#include "histogram.h"

#include "TFile.h"

#include <iomanip>
#include <iostream>
#include <stdio.h>

using Hit = SinglePhotonHit<3, 3>;

int main() {
    constexpr int n_items = 1'000'000'000;
    constexpr int n_iter = 1'000;
    constexpr int buffer_size = n_items / n_iter;
    std::cout << "Buffer size: " << buffer_size << '\n';

    auto th2 = make_hist("name", "title");

    std::vector<Hit> hits(buffer_size);

    FILE *pFile = fopen("/home/l_frojdh/tmp/Module77_ST_Pb75keV_-wCM-dHC.clust", "rb");
    if (pFile) {
        sls::Timer timer;
        for (int i = 0; i != n_iter; ++i) {
            std::fread(hits.data(), sizeof(Hit), buffer_size, pFile);
            // for (auto const &h : hits) {
            //     th2->Fill(h.x * 256 + h.y, h.tot);
            // }
        }
        auto duration = timer.elapsed_s();
        auto throughput  =  static_cast<double>(n_items)*sizeof(Hit)/duration/(1024.*1024.);
        printf("Duration: %.2f s processed %.2f MB/s\n", duration, throughput);
        fclose(pFile);
    } else {
        std::cout << "ERROR: Could not open file\n";
    }

    //Write out data for verification
    auto projy = th2->ProjectionY();
    auto pixel = th2->ProjectionY("pixel5000", 5000, 5001);
    TFile *f = new TFile("test1.root", "RECREATE");
    projy->Write();
    pixel->Write();
    f->Close();
}