#ifndef PATIENT_H
#define PATIENT_H

#include <vector>
#include <array>
#include <random>

class Patient{
    public:
        int arrival_time;
        int pathway;
        int base_duration;
        int service_duration;
        std::vector<int> appt_epochs;
        int extended = 0;
        double extend_prob;
        int discharge_time = 0;
        int total_wait_time = 0;
        std::mt19937 rng;
        std::uniform_real_distribution<double> dis;

        Patient(int arr_t, int path, int duration, double extend_prob, std::mt19937 &gen);

        void add_appt(int epoch);
        void add_wait(int wlist_arr_t, int add_t);
        std::array<bool, 2> process_patient(int epoch);

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
        std::array<bool, 2> check_complete();
};
#endif