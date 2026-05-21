#ifndef WAREHOUSE_APP_SRC_MENU_H_
#define WAREHOUSE_APP_SRC_MENU_H_

#include "SqliteStorage.h"
#include "Warehouse.h"

#include <string>
#include <vector>

class Menu {
 public:
  Menu(Warehouse* warehouse, std::string txt_path, std::string sqlite_path);
  void Run();

 private:
  void PrintMainMenu() const;
  void PrintProducts(const std::vector<Product>& products) const;
  void PrintAllProducts() const;

  void AddProduct();
  void EditProduct();
  void RemoveProduct();
  void SearchProducts();
  void FilterProducts();
  void SortProducts();
  void ShowAnalytics();
  void SaveToTxt();
  void SaveToSqlite();
  void LoadFromSqlite();

  int ReadInt(const std::string& prompt) const;
  double ReadDouble(const std::string& prompt) const;
  std::string ReadText(const std::string& prompt) const;
  bool ReadAscendingChoice() const;

  Warehouse* warehouse_;
  std::string txt_path_;
  SqliteStorage sqlite_storage_;
};

#endif  // WAREHOUSE_APP_SRC_MENU_H_
