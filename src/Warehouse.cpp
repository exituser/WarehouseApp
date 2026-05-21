#include "Warehouse.h"

#include <algorithm>
#include <cctype>
#include <iterator>

bool Warehouse::AddProduct(const Product& product, std::string* error) {
  if (!ValidateProduct(product, error)) {
    return false;
  }

  products_.push_back(product);
  return true;
}

bool Warehouse::UpdateProduct(int id, const std::string& name,
                              const std::string& category, double price,
                              int quantity, std::string* error) {
  Product* product = FindById(id);
  if (product == nullptr) {
    if (error != nullptr) {
      *error = "Товар с таким ID не найден.";
    }
    return false;
  }

  Product updated(id, Trim(name), Trim(category), price, quantity);
  if (updated.name().empty() || updated.category().empty()) {
    if (error != nullptr) {
      *error = "Название и категория не должны быть пустыми.";
    }
    return false;
  }
  if (updated.price() < 0.0 || updated.quantity() < 0) {
    if (error != nullptr) {
      *error = "Цена и количество не могут быть отрицательными.";
    }
    return false;
  }

  product->set_name(updated.name());
  product->set_category(updated.category());
  product->set_price(updated.price());
  product->set_quantity(updated.quantity());
  return true;
}

bool Warehouse::RemoveById(int id) {
  const auto old_size = products_.size();
  products_.erase(std::remove_if(products_.begin(), products_.end(),
                                 [id](const Product& product) {
                                   return product.id() == id;
                                 }),
                  products_.end());
  return products_.size() != old_size;
}

int Warehouse::RemoveByName(const std::string& name) {
  const std::string normalized_name = ToLower(Trim(name));
  const auto old_size = products_.size();
  products_.erase(std::remove_if(products_.begin(), products_.end(),
                                 [&normalized_name](const Product& product) {
                                   return ToLower(product.name()) ==
                                          normalized_name;
                                 }),
                  products_.end());
  return static_cast<int>(old_size - products_.size());
}

Product* Warehouse::FindById(int id) {
  auto it = std::find_if(products_.begin(), products_.end(),
                         [id](const Product& product) {
                           return product.id() == id;
                         });
  return it == products_.end() ? nullptr : &(*it);
}

const Product* Warehouse::FindById(int id) const {
  auto it = std::find_if(products_.begin(), products_.end(),
                         [id](const Product& product) {
                           return product.id() == id;
                         });
  return it == products_.end() ? nullptr : &(*it);
}

std::vector<Product> Warehouse::SearchByName(const std::string& query) const {
  const std::string normalized_query = ToLower(Trim(query));
  std::vector<Product> result;
  std::copy_if(products_.begin(), products_.end(), std::back_inserter(result),
               [&normalized_query](const Product& product) {
                 return ToLower(product.name()).find(normalized_query) !=
                        std::string::npos;
               });
  return result;
}

std::vector<Product> Warehouse::FilterByCategory(
    const std::string& category) const {
  const std::string normalized_category = ToLower(Trim(category));
  std::vector<Product> result;
  std::copy_if(products_.begin(), products_.end(), std::back_inserter(result),
               [&normalized_category](const Product& product) {
                 return ToLower(product.category()) == normalized_category;
               });
  return result;
}

std::vector<Product> Warehouse::FilterByPriceRange(double min_price,
                                                   double max_price) const {
  std::vector<Product> result;
  std::copy_if(products_.begin(), products_.end(), std::back_inserter(result),
               [min_price, max_price](const Product& product) {
                 return product.price() >= min_price &&
                        product.price() <= max_price;
               });
  return result;
}

void Warehouse::SortByPrice(bool ascending) {
  std::sort(products_.begin(), products_.end(),
            [ascending](const Product& left, const Product& right) {
              return ascending ? left.price() < right.price()
                               : left.price() > right.price();
            });
}

void Warehouse::SortByQuantity(bool ascending) {
  std::sort(products_.begin(), products_.end(),
            [ascending](const Product& left, const Product& right) {
              return ascending ? left.quantity() < right.quantity()
                               : left.quantity() > right.quantity();
            });
}

void Warehouse::SortByName(bool ascending) {
  std::sort(products_.begin(), products_.end(),
            [ascending](const Product& left, const Product& right) {
              return ascending ? ToLower(left.name()) < ToLower(right.name())
                               : ToLower(left.name()) > ToLower(right.name());
            });
}

double Warehouse::TotalInventoryValue() const {
  double total = 0.0;
  for (const Product& product : products_) {
    total += product.price() * product.quantity();
  }
  return total;
}

std::vector<Product> Warehouse::LowStockProducts(int threshold) const {
  std::vector<Product> result;
  std::copy_if(products_.begin(), products_.end(), std::back_inserter(result),
               [threshold](const Product& product) {
                 return product.quantity() <= threshold;
               });
  return result;
}

std::vector<Product> Warehouse::TopExpensiveProducts(std::size_t count) const {
  std::vector<Product> result = products_;
  std::sort(result.begin(), result.end(),
            [](const Product& left, const Product& right) {
              return left.price() > right.price();
            });
  if (result.size() > count) {
    result.resize(count);
  }
  return result;
}

const std::vector<Product>& Warehouse::products() const { return products_; }

void Warehouse::ReplaceAll(const std::vector<Product>& products) {
  products_.clear();
  for (const Product& product : products) {
    std::string ignored_error;
    AddProduct(product, &ignored_error);
  }
}

bool Warehouse::IsIdUnique(int id) const { return FindById(id) == nullptr; }

bool Warehouse::ValidateProduct(const Product& product,
                                std::string* error) const {
  if (product.id() <= 0) {
    if (error != nullptr) {
      *error = "ID должен быть положительным числом.";
    }
    return false;
  }
  if (!IsIdUnique(product.id())) {
    if (error != nullptr) {
      *error = "ID должен быть уникальным.";
    }
    return false;
  }
  if (Trim(product.name()).empty() || Trim(product.category()).empty()) {
    if (error != nullptr) {
      *error = "Название и категория не должны быть пустыми.";
    }
    return false;
  }
  if (product.price() < 0.0 || product.quantity() < 0) {
    if (error != nullptr) {
      *error = "Цена и количество не могут быть отрицательными.";
    }
    return false;
  }
  return true;
}

std::string ToLower(std::string text) {
  std::string result;
  result.reserve(text.size());

  for (std::size_t index = 0; index < text.size(); ++index) {
    const unsigned char current = static_cast<unsigned char>(text[index]);
    const unsigned char next =
        index + 1 < text.size() ? static_cast<unsigned char>(text[index + 1])
                                : 0;

    if (current == 0xD0 && next >= 0x90 && next <= 0x9F) {
      result.push_back(static_cast<char>(0xD0));
      result.push_back(static_cast<char>(next + 0x20));
      ++index;
    } else if (current == 0xD0 && next >= 0xA0 && next <= 0xAF) {
      result.push_back(static_cast<char>(0xD1));
      result.push_back(static_cast<char>(next - 0x20));
      ++index;
    } else if (current == 0xD0 && next == 0x81) {
      result.push_back(static_cast<char>(0xD1));
      result.push_back(static_cast<char>(0x91));
      ++index;
    } else {
      result.push_back(static_cast<char>(std::tolower(current)));
    }
  }

  return result;
}

std::string Trim(const std::string& text) {
  const auto begin = std::find_if_not(text.begin(), text.end(),
                                      [](unsigned char character) {
                                        return std::isspace(character) != 0;
                                      });
  const auto end = std::find_if_not(text.rbegin(), text.rend(),
                                    [](unsigned char character) {
                                      return std::isspace(character) != 0;
                                    })
                       .base();
  return begin >= end ? std::string() : std::string(begin, end);
}
