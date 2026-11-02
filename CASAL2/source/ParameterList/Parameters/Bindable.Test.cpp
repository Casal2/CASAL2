/**
 * @file Bindable.Test.cpp
 * @github https://github.com/Zaita
 * @date 30/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 */
#ifdef TESTMODE

// Headers
#include "Bindable.h"

#include <gtest/gtest.h>

#include <cstring>
#include <new>

#include "Bindable-inl.h"

// Namespaces
namespace niwa {
namespace parameters {

/**
 * Bind() assigns the default to an optional parameter with no value. Without a set_default_value()
 * that has to be a value-initialised T rather than whatever was in memory, so the Bindable is built
 * over poisoned storage: otherwise this passes by luck when the heap happens to be zeroed.
 */
TEST(Bindable, OptionalWithoutDefault_BindsToZero) {
  alignas(Bindable<unsigned>) unsigned char storage[sizeof(Bindable<unsigned>)];
  std::memset(storage, 0xAB, sizeof(storage));

  unsigned            target    = 7;
  Bindable<unsigned>* parameter = new (storage) Bindable<unsigned>("label", &target, "description");
  parameter->set_is_optional(true);
  parameter->Bind();
  EXPECT_EQ(0u, target);

  parameter->~Bindable<unsigned>();
}

TEST(Bindable, OptionalWithDefault_BindsToDefault) {
  unsigned           target = 7;
  Bindable<unsigned> parameter("label", &target, "description");
  parameter.set_default_value(3u);
  parameter.Bind();
  EXPECT_EQ(3u, target);
}

} /* namespace parameters */
} /* namespace niwa */
#endif /* TESTMODE */
