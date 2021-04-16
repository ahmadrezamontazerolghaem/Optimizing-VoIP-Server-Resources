#include "memory_pool.hpp"

#include <algorithm>
#include <catch.hpp>
#include <random>
#include <vector>
 
#include "allocator_storage.hpp"
#include "test_allocator.hpp"

using namespace foonathan::memory;

// don't test actual node allocationg, but the connection between block_list and the implementation
// so only  test for memory_pool<node_pool>
TEST_CASE("memory_pool", "[pool]")
{
    using pool_type = memory_pool<node_pool, allocator_reference<test_allocator>>;
    test_allocator alloc;
    {
        pool_type pool(4, 100, alloc);
        REQUIRE(pool.node_size() >= 4u);
        REQUIRE(pool.capacity() <= 100u);
        REQUIRE(pool.next_capacity() >= 100u);
        REQUIRE(&pool.get_allocator().get_allocator() == &alloc);
        REQUIRE(alloc.no_allocated() == 1u);

        SECTION("normal alloc/dealloc")
        {
            std::vector<void*> ptrs;
            auto capacity = pool.capacity();
            for (std::size_t i = 0u; i != capacity / pool.node_size(); ++i)
                ptrs.push_back(pool.allocate_node());
            REQUIRE(pool.capacity() == 0u);
            REQUIRE(alloc.no_allocated() == 1u);

            std::shuffle(ptrs.begin(), ptrs.end(), std::mt19937{});

            for (auto ptr : ptrs)
                pool.deallocate_node(ptr);
            REQUIRE(pool.capacity() == capacity);
        }
        SECTION("multiple block alloc/dealloc")
        {
            std::vector<void*> ptrs;
            auto capacity = pool.capacity();
            for (std::size_t i = 0u; i != capacity / pool.node_size(); ++i)
                ptrs.push_back(pool.allocate_node());
            REQUIRE(pool.capacity() == 0u);

            ptrs.push_back(pool.allocate_node());
            REQUIRE(pool.capacity() >= capacity - pool.node_size());
            REQUIRE(alloc.no_allocated() == 2u);

            std::shuffle(ptrs.begin(), ptrs.end(), std::mt19937{});

            for (auto ptr : ptrs)
                pool.deallocate_node(ptr);
            REQUIRE(pool.capacity() >= capacity);
            REQUIRE(alloc.no_allocated() == 2u);
        }
    }
    REQUIRE(alloc.no_allocated() == 0u);
}
