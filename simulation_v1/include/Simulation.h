#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <random>
#include "Waitlist.h"
#include "DischargeList.h"
#include "Server.h"

class Simulation{
    public:
        Simulation(int n_epochs, std::vector<int> clinicians, double arr_lam, double service_red_beta, int service_red_cap, double ext_prob_cap, 
                    double ext_prob, double wait_ext_beta, double queue_ext_beta, int ext_pol, std::vector<int> pathways,
                    std::vector<double> probs, std::string path, std::string wl_path);

        void generate_servers();
        void generate_arrivals(int epoch);
        void run();
        void write_parquet(std::string path);
        void write_statistics(std::string path);

        int get_n_admitted();

        int get_n_discharged();
        int get_n_waitlist();

        // member-variable setters
        void set_n_epochs(int n_epochs);
        void set_clinicians(std::vector<int> clin);
        void set_arr_lam(double arr_lam);
        void set_service_red_beta(double service_red_beta);
        void set_service_red_cap(int service_red_cap);
        void set_ext_prob_cap(double ext_prob_cap);
        void set_ext_prob(double ext_prob);
        void set_wait_ext_beta(double wait_ext_beta);
        void set_queue_ext_beta(double queue_ext_beta);
        void set_ext_pol(int ext_pol);
        void set_pathways(std::vector<int> pathways);
        void set_probs(std::vector<double> probs);
        void set_n_classes(int n_classes);
        void set_class_dstb(std::discrete_distribution<> class_dstb);
        void set_discharge_list(std::string path);
        void set_waitlist(int n_classes, std::mt19937 &gen);
        void stream_waitlist(int epoch);

    private:
        int n_epochs;
        std::vector<int> clinicians;
        double arr_lam;
        double service_red_beta;
        int service_red_cap;
        double ext_prob_cap;
        double ext_prob;
        double wait_ext_beta;
        double queue_ext_beta;
        int ext_pol;
        std::vector<int> pathways; // map the integer key to string labels for classes outside of the simulation
        std::vector<double> probs;
        int n_classes;
        Waitlist wl;
        DischargeList dl;
        int n_admitted = 0;
        std::vector<Server> servers;
        std::discrete_distribution<> class_dstb;
        parquet::StreamWriter wl_os;
};
#endif
