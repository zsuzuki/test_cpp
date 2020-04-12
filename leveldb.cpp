// leveldb
// https://github.com/google/leveldb/blob/master/doc/index.md
//
#include <iostream>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>

//
int
main(int argc, char** argv)
{
  leveldb::DB*     db;
  leveldb::Options options;
  options.create_if_missing = true;

  // open
  auto status = leveldb::DB::Open(options, ".testdb", &db);
  if (!status.ok())
  {
    std::cerr << status.ToString() << std::endl;
    return 1;
  }

  if (argc > 1)
  {
    leveldb::Slice key1 = argv[1];
    std::string    value;
    auto           s = db->Get(leveldb::ReadOptions(), key1, &value);
    if (s.ok())
    {
      std::cout << key1.ToString() << ": " << value << std::endl;
    }
    else if (argc > 2)
    {
      value  = argv[2];
      auto s = db->Put(leveldb::WriteOptions(), key1, value);
      if (!s.ok())
      {
        std::cerr << "key[" << key1.ToString() << "]: write failed." << std::endl;
        return 1;
      }
      std::cout << "key[" << key1.ToString() << "]: write value<" << value << ">" << std::endl;
    }
    else
    {
      std::cerr << "key[" << key1.ToString() << "]: not found." << std::endl;
    }
    return 0;
  }

  // list
  int  ret = 0;
  auto it  = db->NewIterator(leveldb::ReadOptions());
  for (it->SeekToFirst(); it->Valid(); it->Next())
  {
    std::cout << it->key().ToString() << ": " << it->value().ToString() << std::endl;
  }
  if (it->status().ok())
  {
    std::cerr << it->status().ToString() << std::endl;
    ret = 1;
  }
  delete it;
  return ret;
}
