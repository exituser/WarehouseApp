#ifndef WAREHOUSE_APP_SRC_WAREHOUSE_H_
#define WAREHOUSE_APP_SRC_WAREHOUSE_H_

#include "Product.h"

#include <string>
#include <vector>

class Warehouse {
 public:
  bool AddProduct(const Product& product, std::string* error);
  bool UpdateProduct(int id, const std::string& name,
                     const std::string& category, double price, int quantity,
                     std::string* error);
  bool RemoveById(int id);
  int RemoveByName(const std::string& name);

  Product* FindById(int id);
  const Product* FindById(int id) const;

  std::vector<Product> SearchByName(const std::string& query) const;
  std::vector<Product> FilterByCategory(const std::string& category) const;
  std::vector<Product> FilterByPriceRange(double min_price,
                                          double max_price) const;

  void SortByPrice(bool ascending);
  void SortByQuantity(bool ascending);
  void SortByName(bool ascending);

  double TotalInventoryValue() const;
  std::vector<Product> LowStockProducts(int threshold) const;
  std::vector<Product> TopExpensiveProducts(std::size_t count) const;

  const std::vector<Product>& products() const;
  void ReplaceAll(const std::vector<Product>& products);

 private:
  bool IsIdUnique(int id) const;
  bool ValidateProduct(const Product& product, std::string* error) const;

  std::vector<Product> products_;
};

std::string ToLower(std::string text);
std::string Trim(const std::string& text);

#endif  // WAREHOUSE_APP_SRC_WAREHOUSE_H_
