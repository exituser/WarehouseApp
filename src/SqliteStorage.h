#ifndef WAREHOUSE_APP_SRC_SQLITESTORAGE_H_
#define WAREHOUSE_APP_SRC_SQLITESTORAGE_H_

#include "Product.h"

#include <string>
#include <vector>

class SqliteStorage {
 public:
  explicit SqliteStorage(std::string database_path);

  bool SaveProducts(const std::vector<Product>& products, std::string* error);
  bool LoadProducts(std::vector<Product>* products, std::string* error);

 private:
  bool EnsureSchema(void* database, std::string* error);

  std::string database_path_;
};

#endif  // WAREHOUSE_APP_SRC_SQLITESTORAGE_H_
