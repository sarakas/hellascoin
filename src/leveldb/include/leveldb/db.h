



#ifndef STORAGE_LEVELDB_INCLUDE_DB_H_
#define STORAGE_LEVELDB_INCLUDE_DB_H_

#include <stdint.h>
#include <stdio.h>
#include "leveldb/iterator.h"
#include "leveldb/options.h"

namespace leveldb {


static const int kMajorVersion = 1;
static const int kMinorVersion = 13;

struct Options;
struct ReadOptions;
struct WriteOptions;
class WriteBatch;




class Snapshot {
 protected:
  virtual ~Snapshot();
};


struct Range {



  Range() { }
  Range(const Slice& s, const Slice& l) : start(s), limit(l) { }
};




class DB {
 public:





  static Status Open(const Options& options,
                     const std::string& name,
                     DB** dbptr);

  DB() { }
  virtual ~DB();




  virtual Status Put(const WriteOptions& options,
                     const Slice& key,
                     const Slice& value) = 0;





  virtual Status Delete(const WriteOptions& options, const Slice& key) = 0;




  virtual Status Write(const WriteOptions& options, WriteBatch* updates) = 0;








  virtual Status Get(const ReadOptions& options,
                     const Slice& key, std::string* value) = 0;







  virtual Iterator* NewIterator(const ReadOptions& options) = 0;





  virtual const Snapshot* GetSnapshot() = 0;



  virtual void ReleaseSnapshot(const Snapshot* snapshot) = 0;















  virtual bool GetProperty(const Slice& property, std::string* value) = 0;









  virtual void GetApproximateSizes(const Range* range, int n,
                                   uint64_t* sizes) = 0;











  virtual void CompactRange(const Slice* begin, const Slice* end) = 0;

 private:

  DB(const DB&);
  void operator=(const DB&);
};



Status DestroyDB(const std::string& name, const Options& options);





Status RepairDB(const std::string& dbname, const Options& options);




