#ifndef WAREHOUSE_APP_SRC_FILEIO_H_
#define WAREHOUSE_APP_SRC_FILEIO_H_

#include "Product.h"

#include <string>
#include <vector>

class FileIO {
 public:
  static std::vector<Product> LoadProducts(const std::string& path);
  static bool SaveProducts(const std::string& path,
                           const std::vector<Product>& products,
                           std::string* error);
};

#endif  // WAREHOUSE_APP_SRC_FILEIO_H_
