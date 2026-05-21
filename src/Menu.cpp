#include "Menu.h"

#include "FileIO.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>

Menu::Menu(Warehouse* warehouse, std::string txt_path, std::string sqlite_path)
    : warehouse_(warehouse),
      txt_path_(std::move(txt_path)),
      sqlite_storage_(std::move(sqlite_path)) {}

void Menu::Run() {
  while (true) {
    PrintMainMenu();
    const int choice = ReadInt("Выберите пункт: ");
    switch (choice) {
      case 1:
        PrintAllProducts();
        break;
      case 2:
        AddProduct();
        break;
      case 3:
        EditProduct();
        break;
      case 4:
        RemoveProduct();
        break;
      case 5:
        SearchProducts();
        break;
      case 6:
        FilterProducts();
        break;
      case 7:
        SortProducts();
        break;
      case 8:
        ShowAnalytics();
        break;
      case 9:
        SaveToTxt();
        break;
      case 10:
        SaveToSqlite();
        break;
      case 11:
        LoadFromSqlite();
        break;
      case 0:
        std::cout << "Завершение работы.\n";
        return;
      default:
        std::cout << "Нет такого пункта меню.\n";
        break;
    }
  }
}

void Menu::PrintMainMenu() const {
  std::cout << "\n=== Учёт товаров на складе ===\n"
            << "1. Показать все товары\n"
            << "2. Добавить товар\n"
            << "3. Редактировать товар\n"
            << "4. Удалить товар\n"
            << "5. Поиск по названию\n"
            << "6. Фильтрация\n"
            << "7. Сортировка\n"
            << "8. Аналитика\n"
            << "9. Сохранить в TXT\n"
            << "10. Сохранить в SQLite\n"
            << "11. Загрузить из SQLite\n"
            << "0. Выход\n";
}

void Menu::PrintProducts(const std::vector<Product>& products) const {
  if (products.empty()) {
    std::cout << "Список пуст.\n";
    return;
  }

  std::cout << "ID | Название | Категория | Цена | Количество\n";
  std::cout << "------------------------------------------------\n";
  for (const Product& product : products) {
    std::cout << product << '\n';
  }
}

void Menu::PrintAllProducts() const { PrintProducts(warehouse_->products()); }

void Menu::AddProduct() {
  const int id = ReadInt("ID: ");
  const std::string name = ReadText("Название: ");
  const std::string category = ReadText("Категория: ");
  const double price = ReadDouble("Цена: ");
  const int quantity = ReadInt("Количество: ");

  std::string error;
  if (warehouse_->AddProduct(Product(id, name, category, price, quantity),
                             &error)) {
    std::cout << "Товар добавлен.\n";
  } else {
    std::cout << "Ошибка: " << error << '\n';
  }
}

void Menu::EditProduct() {
  const int id = ReadInt("ID товара для редактирования: ");
  const Product* existing = warehouse_->FindById(id);
  if (existing == nullptr) {
    std::cout << "Товар не найден.\n";
    return;
  }

  std::cout << "Текущая запись:\n";
  PrintProducts({*existing});

  const std::string name = ReadText("Новое название: ");
  const std::string category = ReadText("Новая категория: ");
  const double price = ReadDouble("Новая цена: ");
  const int quantity = ReadInt("Новое количество: ");

  std::string error;
  if (warehouse_->UpdateProduct(id, name, category, price, quantity, &error)) {
    std::cout << "Товар обновлён.\n";
  } else {
    std::cout << "Ошибка: " << error << '\n';
  }
}

void Menu::RemoveProduct() {
  std::cout << "1. Удалить по ID\n"
            << "2. Удалить по названию\n";
  const int choice = ReadInt("Выберите способ: ");
  if (choice == 1) {
    const int id = ReadInt("ID: ");
    std::cout << (warehouse_->RemoveById(id) ? "Товар удалён.\n"
                                             : "Товар не найден.\n");
  } else if (choice == 2) {
    const std::string name = ReadText("Название: ");
    const int removed_count = warehouse_->RemoveByName(name);
    std::cout << "Удалено записей: " << removed_count << '\n';
  } else {
    std::cout << "Нет такого способа удаления.\n";
  }
}

void Menu::SearchProducts() {
  const std::string query = ReadText("Введите часть названия: ");
  PrintProducts(warehouse_->SearchByName(query));
}

void Menu::FilterProducts() {
  std::cout << "1. По категории\n"
            << "2. По диапазону цен\n";
  const int choice = ReadInt("Выберите фильтр: ");
  if (choice == 1) {
    const std::string category = ReadText("Категория: ");
    PrintProducts(warehouse_->FilterByCategory(category));
  } else if (choice == 2) {
    double min_price = ReadDouble("Цена от: ");
    double max_price = ReadDouble("Цена до: ");
    if (min_price > max_price) {
      std::swap(min_price, max_price);
    }
    PrintProducts(warehouse_->FilterByPriceRange(min_price, max_price));
  } else {
    std::cout << "Нет такого фильтра.\n";
  }
}

void Menu::SortProducts() {
  std::cout << "1. По цене\n"
            << "2. По количеству\n"
            << "3. По названию\n";
  const int choice = ReadInt("Выберите сортировку: ");
  const bool ascending = ReadAscendingChoice();

  if (choice == 1) {
    warehouse_->SortByPrice(ascending);
  } else if (choice == 2) {
    warehouse_->SortByQuantity(ascending);
  } else if (choice == 3) {
    warehouse_->SortByName(ascending);
  } else {
    std::cout << "Нет такой сортировки.\n";
    return;
  }

  std::cout << "Список отсортирован.\n";
  PrintAllProducts();
}

void Menu::ShowAnalytics() {
  std::cout << std::fixed << std::setprecision(2)
            << "Общая стоимость склада: "
            << warehouse_->TotalInventoryValue() << '\n';

  std::cout << "\nТовары с остатком <= 5:\n";
  PrintProducts(warehouse_->LowStockProducts(5));

  std::cout << "\nТоп-3 самых дорогих позиции:\n";
  PrintProducts(warehouse_->TopExpensiveProducts(3));
}

void Menu::SaveToTxt() {
  std::string error;
  if (FileIO::SaveProducts(txt_path_, warehouse_->products(), &error)) {
    std::cout << "Данные сохранены в " << txt_path_ << ".\n";
  } else {
    std::cout << "Ошибка сохранения: " << error << '\n';
  }
}

void Menu::SaveToSqlite() {
  std::string error;
  if (sqlite_storage_.SaveProducts(warehouse_->products(), &error)) {
    std::cout << "Данные сохранены в SQLite.\n";
  } else {
    std::cout << "Ошибка SQLite: " << error << '\n';
  }
}

void Menu::LoadFromSqlite() {
  std::vector<Product> products;
  std::string error;
  if (!sqlite_storage_.LoadProducts(&products, &error)) {
    std::cout << "Ошибка SQLite: " << error << '\n';
    return;
  }

  warehouse_->ReplaceAll(products);
  std::cout << "Загружено из SQLite: " << warehouse_->products().size()
            << " записей.\n";
}

int Menu::ReadInt(const std::string& prompt) const {
  while (true) {
    std::cout << prompt;
    int value = 0;
    if (std::cin >> value) {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return value;
    }
    std::cout << "Введите целое число.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

double Menu::ReadDouble(const std::string& prompt) const {
  while (true) {
    std::cout << prompt;
    double value = 0.0;
    if (std::cin >> value) {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return value;
    }
    std::cout << "Введите число.\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

std::string Menu::ReadText(const std::string& prompt) const {
  while (true) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    value = Trim(value);
    if (!value.empty()) {
      return value;
    }
    std::cout << "Значение не должно быть пустым.\n";
  }
}

bool Menu::ReadAscendingChoice() const {
  std::cout << "1. По возрастанию\n"
            << "2. По убыванию\n";
  while (true) {
    const int choice = ReadInt("Выберите направление: ");
    if (choice == 1) {
      return true;
    }
    if (choice == 2) {
      return false;
    }
    std::cout << "Введите 1 или 2.\n";
  }
}
