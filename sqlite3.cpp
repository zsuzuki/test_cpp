#include <iostream>
#include <sqlite3.h>
#include <string>
#include <strstream>

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

  std::strstream sql;
  sql << "select * from test";
  if (argc > 1)
  {
    if (argc > 4)
    {
      // insert
      std::strstream v;
      v << "insert into test values(";
      v << argv[1] << ", '" << argv[2] << "', " << argv[3] << ", '" << argv[4];
      v << "')";
      auto str = v.str();
      sqlite3_exec(db, str, 0, 0, &errmsg);
      return 0;
    }
    else
    {
      sql << " " << argv[1];
    }
  }

  rc = sqlite3_exec(db, sql.str(),
                    [](void*, int argc, char** argv, char** col) {
                      int i;
                      for (i = 0; i < argc; i++)
                        std::cout << col[i] << " = " << (argv[i] ? argv[i] : "null") << " /";
                      std::cout << std::endl;
                      return 0;
                    },
                    0, &errmsg);
  std::cout << "req done." << std::endl;
  if (rc != 0)
  {
    std::cerr << "error: " << errmsg << std::endl;
  }

  // データベースを閉じる
  sqlite3_close(db);
  return 0;
}
