#include "LimitedDataQueue.h"
#include "SinglePhotonHit.h"
#include "TFile.h"
#include "TROOT.h"
#include "Timer.h"
#include "histogram.h"

#include "Reader.h"

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <thread>

using Hit = SinglePhotonHit<3, 3>;
using Data = std::vector<Hit>;
using Queue = LimitedDataQueue<Data>;

constexpr int n_items = 1'000'000'000;
constexpr int n_iter = 1'000;
constexpr int buffer_size = n_items / n_iter;

void fill_histogram(TH2 *h, Reader<Hit> *rr, int id) {
    printf("fill_histogram: %d\n", id);
    int counter = 0;
    std::vector<Hit> hits(buffer_size);
    while (rr->num_reads() < n_iter) {
        rr->read(hits.data());
        for (auto const& hit: hits){
            h->Fill(hit.x*256+hit.y, hit.tot);
        }
    }
    printf("fill_histogram: %d items: %d DONE!\n", id, counter);
}

int main() {
    std::cout << "Buffer size: " << buffer_size << " items  "<< (double)buffer_size*sizeof(Hit)/(1024.*1024.)<<"MB\n";
    ROOT::EnableThreadSafety();

    auto th2 = make_hist("name", "title");

    sls::Timer timer;

    Reader<Hit> r{buffer_size};
    r.open("/home/l_frojdh/tmp/Module77_ST_Pb75keV_-wCM-dHC.clust");

    std::vector<std::thread> threads;
    std::vector<TH2D*> histograms;

    for (int i = 0; i != 4; ++i) {
        histograms.push_back(make_hist("name_"+std::to_string(i), "title"));
        threads.push_back(std::thread(fill_histogram, histograms[i], &r, i));
    }

    for (auto& t: threads){
        t.join();
    }
    // timer.print_elapsed();
    for (auto h: histograms){
        th2->Add(h);
    }

    auto duration = timer.elapsed_s();
    auto throughput  =  static_cast<double>(n_items)*sizeof(Hit)/duration/(1024.*1024.);
    printf("Duration: %.2f s processed %.2f MB/s\n", duration, throughput);

    //Write out data for verification
    auto projy = th2->ProjectionY();
    auto pixel = th2->ProjectionY("pixel5000", 5000, 5001);
    TFile *f = new TFile("test3.root", "RECREATE");
    projy->Write();
    pixel->Write();
    f->Close();
}