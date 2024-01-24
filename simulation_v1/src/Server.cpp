#include "Server.h"
#include <deque>
#include "Patient.h"
#include "Waitlist.h"
#include "DischargeList.h"

Server::Server(Waitlist &wl, DischargeList &dl, int ext) : waitlist(wl), discharge_list(dl) {
    n_patients = 0;
    ext_prot = ext;
}

void Server::add_patient(Patient &patient) {
    caseload.push_back(patient);
    n_patients += 1;
    if (caseload.size() > n_patients){
        std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
        throw std::runtime_error("Error - Line 17");
    }
}

void Server::add_from_waitlist(int epoch){
    if (waitlist.check_waitlist()) {
        std::pair<Patient, int> pair = waitlist.get_patient();
        pair.first.add_wait(pair.second, epoch);
        add_patient(pair.first);
    } else if (logging) {
        std::cout << "No patients on waitlist to add." << std::endl;
    }
}

void Server::process_extension(Patient patient, int epoch){
    switch (ext_prot) {
        case 0: //standard
            caseload.push_back(patient);
            if (caseload.size() > n_patients){
                std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
                throw std::runtime_error("Error - Line 37");
            }
            break;
        case 1: //reduced freq
            if (n_patients < max_caseload) {
                add_from_waitlist(epoch+1);
                caseload.push_back(patient);
                if (caseload.size() > n_patients){
                    std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
                    throw std::runtime_error("Error - Line 45");
                }
            } else {
                caseload.push_back(patient);
                if (caseload.size() > n_patients){
                    std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
                    throw std::runtime_error("Error - Line 51");
                }
            }
            break;
        case 2: //back of waitlist
            waitlist.add_patient(patient, epoch);
            n_patients -= 1;
            break;
    }
}

void Server::process_epoch(int epoch){
    // std::cout << "Processing epoch. n_patients: " << n_patients << " Caseload len: " << caseload.size() << std::endl;
    if (n_patients == 0) {
        if (caseload.size() > n_patients) {
            std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
            throw std::runtime_error("Error - Line 65");
        }
        add_from_waitlist(epoch);
    } else {
        Patient p = caseload.front();
        caseload.pop_front();
        std::array<bool, 2> check = p.process_patient(epoch);
        if (check[0]) { // if they have reached their service_max
            if (!check[1]) { // if they ARE NOT extended
                p.set_discharge_time(epoch);
                discharge_list.add_patient(p);
                n_patients -= 1;
            } else { // if they are extended
                Server::process_extension(p, epoch);
            }
        } else {
            caseload.push_back(p);
            if (caseload.size() > n_patients) {
                std::cout << "Caseload size: " << caseload.size() << " n_patients: " << n_patients << std::endl;
                throw std::runtime_error("Error - Line 84");
            }
        }
    }
}

void Server::print_patients() {
    if (caseload.size() > 0) {
        for (auto p : caseload) {
            std::cout << p.get_total_wait_time();
        }
    } else if (logging) {
        std::cout << "No patients on caseload.";
    }
}