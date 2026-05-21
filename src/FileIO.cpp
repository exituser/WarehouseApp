#include "FileIO.h"

#include "Warehouse.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

bool ParseProductLine(const std::string& line, Product* product) {
  std::stringstream stream(line);
  std::string id_text;
  std::string name;
  std::string category;
  std::string price_text;
  std::string quantity_text;

  if (!std::getline(stream, id_text, ',') || !std::getline(stream, name, ',') ||
      !std::getline(stream, category, ',') ||
      !std::getline(stream, price_text, ',') ||
      !std::getline(stream, quantity_text)) {
    return false;
  }

  try {
    const int id = std::stoi(Trim(id_text));
    const double price = std::stod(Trim(price_text));
    const int quantity = std::stoi(Trim(quantity_text));
    *product = Product(id, Trim(name), Trim(category), price, quantity);
  } catch (const std::exception&) {
    return false;
  }

  return true;
}

}  // namespace

std::vector<Product> FileIO::LoadProducts(const std::string& path) {
  std::vector<Product> products;
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cout << "Файл " << path
              << " не найден. Будет создан при сохранении.\n";
    return products;
  }

  Warehouse validator;
  std::string line;
  int line_number = 0;
  while (std::getline(file, line)) {
    ++line_number;
    if (Trim(line).empty()) {
      continue;
    }

    Product product;
    std::string error;
    if (!ParseProductLine(line, &product) ||
        !validator.AddProduct(product, &error)) {
      std::cout << "Строка " << line_number
                << " пропущена: некорректный формат или данные.\n";
      continue;
    }
  }

  products = validator.products();
  return products;
}

bool FileIO::SaveProducts(const std::string& path,
                          const std::vector<Product>& products,
                          std::string* error) {
  const std::filesystem::path file_path(path);
  const std::filesystem::path parent = file_path.parent_path();
  if (!parent.empty()) {
    std::filesystem::create_directories(parent);
  }

  std::ofstream file(path);
  if (!file.is_open()) {
    if (error != nullptr) {
      *error = "Не удалось открыть файл для записи.";
    }
    return false;
  }

  for (const Product& product : products) {
    file << product.id() << ',' << product.name() << ',' << product.category()
         << ',' << product.price() << ',' << product.quantity() << '\n';
  }

  return true;
}
