/**
 * @file Loader.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 9/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef TESTMODE

// Headers
#include "Loader.h"

#include <iostream>

#include "Model/Model.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace configuration {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

// classes
class LoaderTest {
public:
  bool HandleOperators(vector<string> &line_values) {
    Model model;
    Loader loader(model);
    string error = "";
    if (!loader.HandleOperators(line_values, error)) {
      cout << "ERROR: " << error << endl;
      return false;
    }
    return true;
  }
};

/**
 * Tests
 */
TEST(ConfigurationLoader, HandleOperators_1) {
  vector<string> line_values {"male,female.immature,mature"};
  vector<string> expected { "male.immature", "male.mature",
                            "female.immature", "female.mature" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_2) {
  vector<string> line_values {"male,female.immature,mature.2000:2001"};
  vector<string> expected { "male.immature.2000", "male.immature.2001",
                            "male.mature.2000", "male.mature.2001",
                            "female.immature.2000", "female.immature.2001",
                            "female.mature.2000", "female.mature.2001" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_3) {
  vector<string> line_values {"10:12"};
  vector<string> expected { "10", "11", "12" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_4) {
  vector<string> line_values {"3:1"};
  vector<string> expected { "3", "2", "1" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_5) {
  vector<string> line_values {"1:3.male,female"};
  vector<string> expected { "1.male", "1.female",
    "2.male", "2.female",
    "3.male", "3.female" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_6) {
  vector<string> line_values {"male,female.immature,mature.untag,2000:2001"};
  vector<string> expected {
    "male.immature.untag", "male.immature.2000", "male.immature.2001",
    "male.mature.untag", "male.mature.2000", "male.mature.2001",
    "female.immature.untag", "female.immature.2000", "female.immature.2001",
    "female.mature.untag", "female.mature.2000", "female.mature.2001"
  };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_7) {
  vector<string> line_values {"male.age_length*15"};
  vector<string> expected {
    "male.age_length", "male.age_length", "male.age_length",
    "male.age_length", "male.age_length", "male.age_length",
    "male.age_length", "male.age_length", "male.age_length",
    "male.age_length", "male.age_length", "male.age_length",
    "male.age_length", "male.age_length", "male.age_length"
  };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_8) {
  vector<string> line_values {"male.2001:2002,untagged*2"};
  vector<string> expected {
    "male.2001", "male.2002", "male.untagged", "male.untagged"
  };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_9) {
  vector<string> line_values {"male.2001:2002,untagged*2"};
  vector<string> expected {
    "male.2001", "male.2002", "male.untagged", "male.untagged"
  };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_10) {
  vector<string> line_values {"male.untagged*2,2001:2002"};
  vector<string> expected {
    "male.untagged", "male.untagged", "male.2001", "male.2002"
  };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_11) {
  vector<string> line_values { "male" };
  vector<string> expected { "male" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_12) {
  vector<string> line_values { "male,female,1e-5" };
  vector<string> expected { "male", "female", "1e-5" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_13) {
  vector<string> line_values { "format=male.*.*" };
  vector<string> expected { "format=male.*.*" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_14) {
  vector<string> line_values { "sex=male" };
  vector<string> expected { "sex=male" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_15) {
  vector<string> line_values { "*" };
  vector<string> expected { "*" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_16) {
  vector<string> line_values { "*+" };
  vector<string> expected { "*+" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_17) {
  vector<string> line_values { "sex:male=age_length_male" };
  vector<string> expected { "sex:male=age_length_male" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_18) {
  vector<string> line_values { "sex:male=male.age_length" };
  vector<string> expected { "sex:male=male.age_length" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

TEST(ConfigurationLoader, HandleOperators_19) {
  vector<string> line_values { "format:*.mature=male.age_length" };
  vector<string> expected { "format:*.mature=male.age_length" };

  LoaderTest loader;
  loader.HandleOperators(line_values);

  ASSERT_EQ(expected.size(), line_values.size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(expected[i], line_values[i]) << " with i = " << i;
  }
}

} /* namespace configuration */
} /* namespace niwa */

#endif /* TESTMODE */
