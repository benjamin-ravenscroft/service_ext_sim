#ifndef DISCHARGELIST_H
#define DISCHARGELIST_H

#include <vector>
#include "Patient.h"

#include "arrow/io/file.h"
#include "parquet/stream_writer.h" 

class DischargeList{
    public:
        std::vector<Patient> discharge_list;
        std::string path;
        parquet::StreamWriter os;
        int n_patients = 0;

        DischargeList();
        DischargeList(std::string p);

        void add_patient(Patient patient);
        int get_n_patients();
        int size();
};
#endif