// a quick way to check if data loaded in hepnos or not. 
#include <iostream>
#include <string>
#include <hepnos.hpp>
#include "../src/serialization/seed_serialization.h"

using namespace hepnos;

int main(int argc, char** argv) {

    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <protocol> <configfile>" << std::endl;
        exit(-1);
    }

    DataStore datastore = DataStore::connect(argv[1], argv[2]);
    // Get the root of the DataStore
    DataSet root = datastore.root();
    // Create a DataSet, a Run, a SubRun, and an Event
    DataSet example8 = root.createDataSet("example8");
    Run run = example8.createRun(1);
    SubRun subrun = run.createSubRun(4);
    Event event = subrun.createEvent(32);
    // Store a product into the event
    {
        std::array<double, 3> Pt     = {1.1,2.1,3.1};
        std::array<double, 3> Dir    = {1.2,2.2,3.2};
        std::array<double, 3> PtErr  = {1.3,2.3,3.3};
        std::array<double, 3> DirErr = {1.4,2.4,3.4};
        recob::Seed s1(Pt.data(), Dir.data(), PtErr.data(), DirErr.data());
        //Particle p("electron", 3.4, 4.5, 5.6);
        ProductID pid = event.store("mylabel", s1);
    }
    // Reload a product from the event
    {
        recob::Seed s2;
        bool b = event.load("mylabel", s2);
        if(b) std::cout << "Seed loaded succesfully" << std::endl;
        else  std::cout << "Particle wasn't loaded" << std::endl;
    }
}
