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

        DischargeList();
        DischargeList(std::string p);

        void add_patient(Patient patient);
        int size();
};
#endif