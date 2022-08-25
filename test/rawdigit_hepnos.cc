#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>
#include "../src/serialization/rawdigit_serialization.h"

#include <hepnos/DataStore.hpp>
#include <hepnos/InputArchive.hpp>
#include <hepnos/OutputArchive.hpp>

#include <iostream>
#include <fstream>

int main() {

  std::vector<short> adcs{2, 4, 6, 8, 9};
  unsigned int channel = 56;
  ULong64_t samples = 78;
  float sigma = 2.3;
  float pdestal = 6.5;
  raw::RawDigit digi(channel,samples, adcs, raw::kNone);

  std::vector<raw::RawDigit> product;
  for(unsigned i=0; i < 5; i++) {
    product.emplace_back(digi);
  }

  {
    std::string product_str;
    {
      hepnos::OutputStringWrapper value_wrapper(product_str);
      hepnos::OutputStream value_stream(value_wrapper);
      hepnos::OutputArchive oa(value_stream);

      try {
        size_t count = product.size();
        oa << count;
        for(unsigned i = 0; i < count; i++)
          oa << product[i];
      } catch(const std::exception& e) {
        throw std::runtime_error(std::string("Exception occured during serialization: ") + e.what());
      }
    }

    std::ofstream ofs("rawdigiout");
    ofs << product_str;
    ofs.close();
  }

  {
    std::ifstream ifs("rawdigiout");
    std::string result_str;
    ifs >> result_str;

    hepnos::InputStringWrapper value_wrapper(result_str.data(), result_str.size());
    hepnos::InputStream value_stream(value_wrapper);
    hepnos::InputArchive ia(hepnos::DataStore(), value_stream);

    std::vector<raw::RawDigit> result;
    size_t count = 0;
    ia >> count;
    std::cout << "Count as read from input is : " << count << "\n";
    result.resize(count);
    for(unsigned i=0; i<count; i++) {
      ia >> result[i];
    }
  }

  return 0;
}
