#include "LimitedDataQueue.h"
#include "SinglePhotonHit.h"
#include "TFile.h"
#include "TROOT.h"
#include "Timer.h"
#include "histogram.h"
#include "ThreadSafeReader.h"

#include "fmt/format.h"

#include <chrono>
#include <iostream>
#include <thread>

using Hit = SinglePhotonHit<3, 3>;
using Data = std::vector<Hit>;
using Queue = LimitedDataQueue<Data>;

// constexpr int n_items = 1'000'000'000;
constexpr int n_items = 10'000'000;
constexpr int n_iter = 10'000;
constexpr int buffer_size = n_items / n_iter;
double buffMB = static_cast<double>(buffer_size)*sizeof(Hit)/(1024.*1024.);


void fill_histogram(TH2D *h, ThreadSafeReader<Hit, buffer_size> *rr, size_t id) {
    fmt::print("Thread {} started\n", id);
    std::vector<Hit> hits(buffer_size);
    while (rr->num_reads() < n_iter) {
        rr->read(hits.data());
        for (auto const& hit: hits){
            h->Fill(hit.x*256+hit.y, hit.tot);
        }
    }
    fmt::print("Thread {} finished\n", id);
}


int main() {
    ROOT::EnableThreadSafety();
    fmt::print("Buffer size: {:d} elements {:.2f} MB\n", buffer_size, buffMB);
    const std::string hist_name("name");
    const std::string fname("/home/l_frojdh/tmp/Module77_ST_Pb75keV_-wCM-dHC.clust");
    // const std::string fname("/home/l_frojdh/tmp/big.clust");

    auto th2 = make_hist(hist_name, "title");
    const int n_threads = 4;

    sls::Timer timer;
    ThreadSafeReader<Hit, buffer_size> r(fname);
    std::vector<std::thread> threads;
    std::vector<TH2D*> histograms;
    
    for (size_t i = 0; i != n_threads; ++i) {
        TH2D* h = static_cast<TH2D*>(th2->Clone());
        h->SetName( fmt::format("{}_x{}", hist_name, i).c_str());
        histograms.push_back(h);
        threads.push_back(std::thread(fill_histogram, histograms[i], &r, i));
    }

    for (auto& t: threads){
        t.join();
    }

    for (auto h: histograms){
        th2->Add(h);
    }

    auto duration = timer.elapsed_s();
    auto throughput  =  static_cast<double>(n_items)*sizeof(Hit)/duration/(1024.*1024.);
    fmt::print("Duration: {:.2f} s Throughput {:.2f} MB/s\n", duration, throughput);
    save_hist("test33.root", th2);
}