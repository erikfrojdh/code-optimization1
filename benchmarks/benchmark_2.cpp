#include "LimitedDataQueue.h"
#include "SinglePhotonHit.h"
#include "TFile.h"
#include "TROOT.h"
#include "Timer.h"
#include "histogram.h"

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <thread>

using Hit = SinglePhotonHit<3, 3>;
using Data = std::vector<Hit>;
using Queue = LimitedDataQueue<Data>;

void fill_histogram(TH2 *h, Queue *qq, int id) {
    printf("fill_histogram: %d\n", id);
    int counter = 0;
    while (qq->producerNotFinished()) {
        Data vec;
        bool popped = qq->wait_pop(vec);
        // printf("popped, qsize: %d\n", qq->size());
        if (popped) {
            for (auto const &hit : vec) {
                h->Fill(hit.x * 256 + hit.y, hit.tot);
            }
            counter++;
        }
    }
    printf("fill_histogram: %d items: %d DONE!\n", id, counter);
}

int main() {
    ROOT::EnableThreadSafety();
    constexpr int n_items = 1'000'000'000;
    constexpr int n_iter = 10'000;
    constexpr int buffer_size = n_items / n_iter;
    std::cout << "Buffer size: " << buffer_size << '\n';

    auto th2 = make_hist("name", "title");

    auto q = Queue{10};
    std::vector<std::thread> threads;
    std::vector<TH2D*> histograms;

    for (int i = 0; i != 8; ++i) {
        histograms.push_back(make_hist("name_"+std::to_string(i), "title"));
        threads.push_back(std::thread(fill_histogram, histograms[i], &q, i));
    }
    FILE *pFile = fopen("/home/l_frojdh/tmp/Module77_ST_Pb75keV_-wCM-dHC.clust", "rb");
    sls::Timer timer;
    
    if (pFile) {

        for (int i = 0; i != n_iter; ++i) {
            std::vector<Hit> hits(buffer_size);
            std::fread(hits.data(), sizeof(Hit), buffer_size, pFile);
            q.wait_push(hits);
        }

        fclose(pFile);
        q.setProducerFinished(true);
    } else {
        std::cout << "ERROR: Could not open file\n";
    }

    for (auto& t: threads){
        t.join();
    }
    timer.print_elapsed();
    for (auto h: histograms){
        th2->Add(h);
    }
    
    // timer.print_elapsed();
    auto duration = timer.elapsed_s();
    auto throughput  =  static_cast<double>(n_items)*sizeof(Hit)/duration/(1024.*1024.);
    printf("Duration: %.2f s processed %.2f MB/s\n", duration, throughput);

    //Write out data for verification
    auto projy = th2->ProjectionY();
    auto pixel = th2->ProjectionY("pixel5000", 5000, 5001);
    TFile *f = new TFile("test2.root", "RECREATE");
    projy->Write();
    pixel->Write();
    f->Close();
}