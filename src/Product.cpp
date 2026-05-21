#include "Product.h"

#include <iomanip>
#include <ostream>
#include <utility>

Product::Product(int id, std::string name, std::string category, double price,
                 int quantity)
    : id_(id),
      name_(std::move(name)),
      category_(std::move(category)),
      price_(price),
      quantity_(quantity) {}

int Product::id() const { return id_; }

const std::string& Product::name() const { return name_; }

const std::string& Product::category() const { return category_; }

double Product::price() const { return price_; }

int Product::quantity() const { return quantity_; }

void Product::set_name(const std::string& name) { name_ = name; }

void Product::set_category(const std::string& category) { category_ = category; }

void Product::set_price(double price) { price_ = price; }

void Product::set_quantity(int quantity) { quantity_ = quantity; }

std::ostream& operator<<(std::ostream& os, const Product& product) {
  const std::ios::fmtflags old_flags = os.flags();
  const std::streamsize old_precision = os.precision();

  os << product.id() << " | " << product.name() << " | "
     << product.category() << " | " << std::fixed << std::setprecision(2)
     << product.price() << " | " << product.quantity();

  os.flags(old_flags);
  os.precision(old_precision);
  return os;
}
