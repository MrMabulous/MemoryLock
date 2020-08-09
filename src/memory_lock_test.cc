#include "memory_lock.h"
#include "gtest/gtest.h"

#include <stdexcept>
#include <vector>

namespace memorylock {

TEST(MemoryLockTest, LockUnlock) {
  std::vector<float> test_vec(100);  

  try {
    LockMemory(&test_vec[0], test_vec.size());
  }
  catch ( std::exception e) {
    ASSERT_TRUE(false) << "Locking failed: " << e.what();
  }

  try {
    UnlockMemory(&test_vec[0], test_vec.size());
  }
  catch ( std::exception e) {
    ASSERT_TRUE(false) << "Locking failed: " << e.what();
  }
}

TEST(MemoryLockTestDeath, LockUnlockBadInput) {
  void* ptr = (void*)123456789;

  EXPECT_THROW(LockMemory(ptr, 1000), std::runtime_error)
      << "since first parameter points to an invalid location";
}

TEST(MemoryLockTest, PrefetchMemory) {
  std::vector<float> test_vec(100);
  std::vector<float> test_vec2(50);
  
  MemoryRange entries[2];
  entries[0].address = &test_vec[0];
  entries[0].length = test_vec.size() * sizeof(float);
  entries[1].address = &test_vec2[0];
  entries[1].length = test_vec2.size() * sizeof(float);

  try {
    PrefetchMemory(entries, 2);
  }
  catch ( std::exception e) {
    ASSERT_TRUE(false) << "PrefetchMemory failed: " << e.what();
  }

  std::vector<MemoryRange> test_vec3(13);
  
  memorylock::MemoryRange entry;
  entry.address = &test_vec3[0];
  entry.length = test_vec3.size() * sizeof(MemoryRange);


  try {
    memorylock::PrefetchMemory(&entry, 1);
  }
  catch ( std::exception e) {
    ASSERT_TRUE(false) << "PrefetchMemory failed: " << e.what();
  }
}

TEST(MemoryLockTestDeath, PrefetchMemoryBadInput) {
  std::vector<float> test_vec(100);
  std::vector<float> test_vec2(50);
  
  MemoryRange entries[2];
  entries[0].address = &test_vec[0];
  entries[0].length = test_vec.size() * sizeof(float);
  entries[1].address = &test_vec2[0];
  entries[1].length = test_vec2.size() * sizeof(float);

  EXPECT_THROW(PrefetchMemory(entries, 3), std::runtime_error)
      << "Expecting throw since second parameter should be 2.";
}

}  // namespace memorylock