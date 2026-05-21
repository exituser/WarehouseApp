#include "FileIO.h"
#include "Menu.h"
#include "Warehouse.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

constexpr const char* kTxtPath = "data/products.txt";
constexpr const char* kSqlitePath = "data/products.db";

}  // namespace

int main() {
#ifdef _WIN32
  SetConsoleCP(65001);
  SetConsoleOutputCP(65001);
#endif

  Warehouse warehouse;
  const std::vector<Product> loaded_products = FileIO::LoadProducts(kTxtPath);
  warehouse.ReplaceAll(loaded_products);

  std::cout << "Автозагрузка TXT: " << warehouse.products().size()
            << " записей.\n";

  Menu menu(&warehouse, kTxtPath, kSqlitePath);
  menu.Run();
  return 0;
}
