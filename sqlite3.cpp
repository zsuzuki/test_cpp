#include <iostream>
#include <sqlite3.h>
#include <string>

int
main(int argc, char** argv)
{
  std::cout << "SQLite version: " << sqlite3_version << std::endl;

  sqlite3* db;
  char*    errmsg;

  int rc = sqlite3_open("test.sqlite3", &db);
  if (rc != 0)
  {
    std::cerr << "error: " << rc << std::endl;
    return 1;
  }

  // sqlite3_exec(db, "insert into test values(0, 'y_suzuki', 10, 'JP')", 0, 0, &errmsg);

  rc = sqlite3_exec(db, "select * from test",
                    [](void*, int argc, char** argv, char** col) {
                      int i;
                      for (i = 0; i < argc; i++)
                        std::cout << col[i] << " = " << (argv[i] ? argv[i] : "null") << " /";
                      std::cout << std::endl;
                      return 0;
                    },
                    0, &errmsg);
  if (rc != 0)
  {
    std::cerr << "error: " << errmsg << std::endl;
  }

  // データベースを閉じる
  sqlite3_close(db);
  return 0;
}
