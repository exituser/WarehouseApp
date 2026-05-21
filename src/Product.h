#ifndef WAREHOUSE_APP_SRC_PRODUCT_H_
#define WAREHOUSE_APP_SRC_PRODUCT_H_

#include <iosfwd>
#include <string>

class Product {
 public:
  Product() = default;
  Product(int id, std::string name, std::string category, double price,
          int quantity);

  int id() const;
  const std::string& name() const;
  const std::string& category() const;
  double price() const;
  int quantity() const;

  void set_name(const std::string& name);
  void set_category(const std::string& category);
  void set_price(double price);
  void set_quantity(int quantity);

 private:
  int id_ = 0;
  std::string name_;
  std::string category_;
  double price_ = 0.0;
  int quantity_ = 0;
};

std::ostream& operator<<(std::ostream& os, const Product& product);

#endif  // WAREHOUSE_APP_SRC_PRODUCT_H_
