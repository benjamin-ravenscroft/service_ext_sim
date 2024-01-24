#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <random>
#include "Waitlist.h"
#include "DischargeList.h"
#include "Server.h"

class Simulation{
    public:
        int n_epochs;
        std::vector<int> clinicians;
        double arr_lam;
        double ext_prob;
        int ext_pol;
        std::vector<int> pathways; // map the integer key to string labels for classes outside of the simulation
        std::vector<double> probs;
        int n_classes;
        Waitlist wl;
        DischargeList dl;

        int n_admitted = 0;

        std::vector<Server> servers;
        std::discrete_distribution<> class_dstb;

        Simulation(int n_ep, std::vector<int> clin, double lam, double ext_p, int pol,
            std::vector<int> serv_path, std::vector<double> serv_prob, std::string path);

        void generate_servers();
        void generate_arrivals(int epoch);
        void run();
        void write_parquet(std::string path);
        void write_statistics(std::string path);

        int get_n_admitted();
};
#endif
