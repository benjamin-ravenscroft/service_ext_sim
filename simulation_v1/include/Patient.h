#ifndef PATIENT_H
#define PATIENT_H

#include <vector>
#include <array>
#include <random>

class Patient{
    public:
        std::mt19937 rng;
        std::uniform_real_distribution<double> dis;

        Patient(int arrival_time, int pathway, int base_duration, double service_red_beta, double ext_prob_cap,
                    double base_ext_p, double wait_ext_beta, double queue_ext_beta, std::mt19937 &gen);

        void add_appt(int epoch);
        void add_wait(int wlist_arr_t, int add_t);
        std::array<bool, 2> process_patient(int epoch, int wl_len);

        // setters
        void set_arrival_time(int t);
        void set_pathway(int p);
        void set_base_duration(int d);
        void set_service_duration(int d);
        void set_service_red_beta(double d);
        void set_ext_prob_cap(double p);
        void set_base_ext_p(double p);
        void set_wait_ext_beta(double b);
        void set_queue_ext_beta(double b);
        void set_extended(int c);

        int get_pathway();
        int get_base_duration();
        int get_arrival_t();
        int get_first_appt();
        int get_n_appts();
        int get_n_ext();
        int get_discharge_time();
        int get_sojourn_time();
        int get_total_wait_time();
        
        void set_discharge_time(int epoch);

    private:
        int arrival_time;
        int pathway;
        int base_duration;
        int service_duration;
        double service_red_beta;
        std::vector<int> appt_epochs;
        int extended = 0;
        double ext_prob_cap;
        double base_ext_p;
        double wait_ext_beta;
        double queue_ext_beta;
        int discharge_time = 0;
        int total_wait_time = 0;
        int ext_cap = 1;

        double calculate_ext_prob();
        std::array<bool, 2> check_complete(int wl_len);
};
#endif