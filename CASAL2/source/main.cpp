#ifdef TESTMODE
#include <gtest/gtest.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
#else

// defines
#ifndef BOOST_USE_WINDOWS_H
#define BOOST_USE_WINDOWS_H
#endif

// Headers
#include <iostream>
#include <thread>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <fenv.h>

#include "Runner.h"

#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Model.h"
#include "Reports/Manager.h"
#include "Utilities/CommandLineParser/CommandLineParser.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/RunParameters.h"
#include "Logging/Logging.h"

// Namespaces
using namespace niwa;
using std::cout;
using std::endl;

/**
 * Application entry point
 */
int main(int argc, char *argv[]) {
	int return_code = 0;
	//feenableexcept(FE_INVALID | FE_OVERFLOW);

	try {
		Runner runner;

		utilities::RunParameters parameters;
		utilities::CommandLineParser parser;
		parser.Parse(argc, argv, parameters);
		runner.global_configuration().set_run_parameters(parameters);
		runner.set_run_parameters(parameters);

		// Store the command line parameters so they can be reported on.
		vector<string> cmd_parameters;
		for (int i = 0; i < argc; ++i)
			cmd_parameters.push_back(argv[i]);
		runner.global_configuration().set_command_line_parameters(cmd_parameters);

		return_code = runner.Go();

	} catch (const string &exception) {
		cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
		cout << "Error: " << exception << endl;
		return_code = -1;

	} catch (std::exception &e) {
		cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
		cout << e.what() << endl;
		return_code = -1;

	} catch (...) {
		cout << "## ERROR - CASAL2 experienced a problem and has stopped execution" << endl;
		cout << "The exception was caught with the catch-all. The type was unknown" << endl;
		cout << "Please contact the application developer" << endl;
		return_code = -1;
	}

	if (return_code == 0) {
		LOG_FINEST() << "Done";
	} else {
		LOG_FINEST() << "Done with errors";
	}

	return return_code;
}
#endif

