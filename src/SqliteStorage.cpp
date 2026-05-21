#include "SqliteStorage.h"

#include "Warehouse.h"

#include <filesystem>
#include <sqlite3.h>
#include <string>
#include <utility>

namespace {

struct DatabaseHandle {
  sqlite3* database = nullptr;

  ~DatabaseHandle() {
    if (database != nullptr) {
      sqlite3_close(database);
    }
  }
};

bool Execute(sqlite3* database, const std::string& sql, std::string* error) {
  char* raw_error = nullptr;
  const int result = sqlite3_exec(database, sql.c_str(), nullptr, nullptr,
                                  &raw_error);
  if (result != SQLITE_OK) {
    if (error != nullptr) {
      *error = raw_error == nullptr ? "Ошибка SQLite." : raw_error;
    }
    sqlite3_free(raw_error);
    return false;
  }
  return true;
}

}  // namespace

SqliteStorage::SqliteStorage(std::string database_path)
    : database_path_(std::move(database_path)) {}

bool SqliteStorage::SaveProducts(const std::vector<Product>& products,
                                 std::string* error) {
  const std::filesystem::path db_path(database_path_);
  if (!db_path.parent_path().empty()) {
    std::filesystem::create_directories(db_path.parent_path());
  }

  DatabaseHandle handle;
  if (sqlite3_open(database_path_.c_str(), &handle.database) != SQLITE_OK) {
    if (error != nullptr) {
      *error = sqlite3_errmsg(handle.database);
    }
    return false;
  }

  if (!EnsureSchema(handle.database, error) ||
      !Execute(handle.database, "BEGIN TRANSACTION;", error) ||
      !Execute(handle.database, "DELETE FROM products;", error)) {
    return false;
  }

  const char* sql =
      "INSERT INTO products (id, name, category, price, quantity) "
      "VALUES (?, ?, ?, ?, ?);";
  sqlite3_stmt* statement = nullptr;
  if (sqlite3_prepare_v2(handle.database, sql, -1, &statement, nullptr) !=
      SQLITE_OK) {
    if (error != nullptr) {
      *error = sqlite3_errmsg(handle.database);
    }
    Execute(handle.database, "ROLLBACK;", nullptr);
    return false;
  }

  for (const Product& product : products) {
    sqlite3_bind_int(statement, 1, product.id());
    sqlite3_bind_text(statement, 2, product.name().c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, product.category().c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 4, product.price());
    sqlite3_bind_int(statement, 5, product.quantity());

    if (sqlite3_step(statement) != SQLITE_DONE) {
      if (error != nullptr) {
        *error = sqlite3_errmsg(handle.database);
      }
      sqlite3_finalize(statement);
      Execute(handle.database, "ROLLBACK;", nullptr);
      return false;
    }
    sqlite3_reset(statement);
    sqlite3_clear_bindings(statement);
  }

  sqlite3_finalize(statement);
  return Execute(handle.database, "COMMIT;", error);
}

bool SqliteStorage::LoadProducts(std::vector<Product>* products,
                                 std::string* error) {
  DatabaseHandle handle;
  if (sqlite3_open(database_path_.c_str(), &handle.database) != SQLITE_OK) {
    if (error != nullptr) {
      *error = sqlite3_errmsg(handle.database);
    }
    return false;
  }

  if (!EnsureSchema(handle.database, error)) {
    return false;
  }

  const char* sql =
      "SELECT id, name, category, price, quantity FROM products ORDER BY id;";
  sqlite3_stmt* statement = nullptr;
  if (sqlite3_prepare_v2(handle.database, sql, -1, &statement, nullptr) !=
      SQLITE_OK) {
    if (error != nullptr) {
      *error = sqlite3_errmsg(handle.database);
    }
    return false;
  }

  Warehouse validator;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    Product product(
        sqlite3_column_int(statement, 0),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 1)),
        reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)),
        sqlite3_column_double(statement, 3), sqlite3_column_int(statement, 4));
    std::string ignored_error;
    validator.AddProduct(product, &ignored_error);
  }

  sqlite3_finalize(statement);
  if (products != nullptr) {
    *products = validator.products();
  }
  return true;
}

bool SqliteStorage::EnsureSchema(void* database, std::string* error) {
  const char* sql =
      "CREATE TABLE IF NOT EXISTS products ("
      "id INTEGER PRIMARY KEY,"
      "name TEXT NOT NULL,"
      "category TEXT NOT NULL,"
      "price REAL NOT NULL,"
      "quantity INTEGER NOT NULL"
      ");";
  return Execute(static_cast<sqlite3*>(database), sql, error);
}
