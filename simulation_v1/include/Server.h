#ifndef SERVER_H
#define SERVER_H

#include <deque>
#include "Patient.h"
#include "Waitlist.h"
#include "DischargeList.h"

class Server{
    public:
        std::deque<Patient> caseload = std::deque<Patient>();
        int n_patients;
        Waitlist& waitlist;
        DischargeList& discharge_list;
        int ext_prot;
        int max_caseload = 5; // max allowable caseload -> impacts freq (ie,, on  bi-weekly sched, max 2 = montly service freq)
        bool logging = false; // variable to use to report if patients are on waitlist or not

        Server(Waitlist &wl, DischargeList &dl, int ext);
        void add_patient(Patient &patient);
        void add_from_waitlist(int epoch);
        void process_extension(Patient patient, int epoch);
        void process_epoch(int epoch);

        void print_patients();

};
#endif