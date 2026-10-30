/**
 * @file MortalityFisheryBase.Test.cpp
 * @github https://github.com/Zaita
 * @date 30/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @description
 * Tests for the catches/method table setup shared by mortality_instantaneous,
 * mortality_instantaneous_retained and mortality_hybrid.
 *
 * The "Discriminating" tests pin the behaviour each class must keep. Their expected text was
 * captured from the code as it stood before the classes shared a base.
 */
#ifdef TESTMODE

// Headers
#include <iomanip>
#include <sstream>

#include "Model/Managers.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyLengthModel.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace processes {
namespace common {

using niwa::testfixtures::InternalEmptyLengthModel;
using niwa::testfixtures::InternalEmptyModel;
using std::ostringstream;

/**
 * Two sexes and two time steps. Each process below fishes in step1 only (except where stated), so
 * step2 exercises the "no F in this time step" path.
 */
const std::string fishery_base_model =
    R"(
@model
start_year 2001
final_year 2005
min_age 1
max_age 10
age_plus true
base_weight_units tonnes
initialisation_phases Define_manually
time_steps step1 step2

@categories
format sex
names male female
age_lengths asMm0 asMm0

@initialisation_phase Define_manually
type state_category_by_age
categories male female
min_age 1
max_age 10
table n
male   100000 90000  80000 70000 60000 50000 40000 30000 20000 10000
female 120000 100000 85000 72000 61000 52000 44000 37000 31000 26000
end_table

@time_step step1
processes mort

@time_step step2
processes mort

@selectivity One
type constant
c 1

@selectivity fifty_percent
type constant
c 0.5

@selectivity fishSel
type logistic
a50 4
ato95 1
length_based False

@selectivity lineSel
type logistic
a50 7
ato95 2
length_based False

@selectivity retSel
type logistic
a50 6
ato95 1
length_based False

@penalty CatchPenalty
type process
log_scale True
multiplier 1000

@age_length asMm0
type von_bertalanffy
by_length true
linf 55.7
k 0.14
t0 -0.82
cv_first 0.1
cv_last 0.1
length_weight Length_Weight

@length_weight Length_Weight
type basic
units kgs
a 0.007289
b 3.2055

@age_weight weights
type Data
units tonnes
table data
year 1 2 3 4 5 6 7 8 9 10
2001 0.1 0.6 1.6 2.7 3.6 4.9 6.3 6.6 7.2 7.5
2002 0.1 0.7 1.5 2.8 4.0 4.8 5.7 6.1 7.1 8.0
2003 0.1 0.6 1.5 2.6 3.7 5.0 5.6 6.3 6.9 8.8
2004 0.1 0.7 1.6 2.4 3.6 5.0 5.5 6.3 6.9 7.2
2005 0.1 0.7 1.6 2.5 3.7 5.1 5.8 6.4 7.0 7.4
end_table
)";

const std::string instantaneous_process =
    R"(
@process mort
type mortality_instantaneous
m 0.1
time_step_proportions 0.6 0.4
relative_m_by_age One
categories *
table catches
year Trawl Line Quota
2003 4000  90000 0.1
2005 70000 30000 0.2
end_table
table method
method category selectivity u_max time_step penalty      biomass u
Trawl  *        fishSel     0.7   step1     CatchPenalty true    false
Line   *        lineSel     0.7   step1     none         false   false
Quota  male     fishSel     0.7   step2     none         true    true
end_table
)";

const std::string retained_process =
    R"(
@process mort
type mortality_instantaneous_retained
m 0.1
relative_m_by_age One
categories *
table catches
year FishingPot FishingLine
2003 3000       1000
2005 90000      2000
end_table
table method
method      category selectivity retained_selectivity discard_mortality_selectivity u_max time_step penalty
FishingPot  *        fishSel     retSel               fifty_percent                 0.7   step1     CatchPenalty
FishingLine *        lineSel     retSel               fifty_percent                 0.7   step1     none
end_table
)";

const std::string hybrid_process =
    R"(
@process mort
type mortality_hybrid
m 0.1
time_step_proportions 0.6 0.4
relative_m_by_age One
categories *
biomass false
table catches
year Trawl Line
2003 20000 5000
2005 40000 8000
end_table
table method
method category selectivity annual_duration time_step penalty
Trawl  *        fishSel     1               step1     CatchPenalty
Line   *        lineSel     1               step1     none
end_table
)";

/**
 * Replace the only occurrence of old_text, failing the test if it isn't there. A typo here would
 * otherwise leave a valid config behind and turn an error-path test into a no-op.
 */
std::string Replaced(std::string text, const std::string& old_text, const std::string& new_text) {
  size_t position = text.find(old_text);
  EXPECT_NE(std::string::npos, position) << "could not find '" << old_text << "'";
  if (position != std::string::npos)
    text.replace(position, old_text.size(), new_text);
  return text;
}

/**
 * The same process with an age_weight_label column on every row
 */
std::string WithAgeWeightColumn(const std::string& process, const std::string& label = "weights") {
  std::string result = process;
  if (result.find("biomass u\n") != std::string::npos) {
    result = Replaced(result, "biomass u\n", "biomass u age_weight_label\n");
    result = Replaced(result, "true    false\n", "true    false " + label + "\n");
    result = Replaced(result, "false   false\n", "false   false " + label + "\n");
    return Replaced(result, "true    true\n", "true    true " + label + "\n");
  }
  if (result.find("biomass false\n") != std::string::npos)
    result = Replaced(result, "biomass false\n", "biomass true\n");
  result = Replaced(result, "time_step penalty\n", "time_step penalty age_weight_label\n");
  result = Replaced(result, "step1     CatchPenalty\n", "step1     CatchPenalty " + label + "\n");
  return Replaced(result, "step1     none\n", "step1     none " + label + "\n");
}

const std::string expected_retained_two_fisheries =
    R"(year: 2001 2002 2003 2004 2005 
fishing_pressure[FishingLine]: 0.00797338 0.0210036 
catch[FishingLine]: 1056.24 2113.65 
actual_catch[FishingLine]: 1056.24 1822.17 
retained_catch[FishingLine]: 1000 2000 
actual_retained_catch[FishingLine]: 1000 1724.2 
discards[FishingLine]: 56.239 97.9783 
discards_dead[FishingLine]: 28.1195 48.9891 
fishing_pressure[FishingPot]: 0.0172144 0.67925 
catch[FishingPot]: 4191.01 126186 
actual_catch[FishingPot]: 4191.01 108784 
retained_catch[FishingPot]: 3000 90000 
actual_retained_catch[FishingPot]: 3000 77588.8 
discards[FishingPot]: 1191.01 31195.5 
discards_dead[FishingPot]: 595.507 15597.7 
male: 36785.9989678 33076.9957143 28916.8938688 21270.0983522 14454.621467 8869.36018621 4720.89419248 3282.47787744 2158.64979619 1075.94518355
female: 44143.1987613 36752.2174603 30724.1997356 21877.815448 14695.5318248 9224.13459366 5192.98361173 4048.38938217 3345.9071841 2797.45747722
)";

const std::string expected_retained_age_weight =
    R"(year: 2001 2002 2003 2004 2005 
fishing_pressure[FishingLine]: 0.0012429 0.00381405 
catch[FishingLine]: 1068.76 2143.23 
actual_catch[FishingLine]: 1068.76 2143.23 
retained_catch[FishingLine]: 1000 2000 
actual_retained_catch[FishingLine]: 1000 2000 
discards[FishingLine]: 68.7592 143.23 
discards_dead[FishingLine]: 4.38331 8.99819 
fishing_pressure[FishingPot]: 0.00259588 0.117706 
catch[FishingPot]: 4456.35 134188 
actual_catch[FishingPot]: 4456.35 134188 
retained_catch[FishingPot]: 3000 90000 
actual_retained_catch[FishingPot]: 3000 90000 
discards[FishingPot]: 1456.35 44187.7 
discards_dead[FishingPot]: 89.8005 2725.44 
male: 36787.607988 33103.5950097 29341.6294423 24974.6530318 20747.1814953 16729.3635928 12958.5282051 9669.99262566 6440.54917525 3219.55780235
female: 44145.1295856 36781.772233 31175.4812825 25688.214547 21092.9678536 17398.5381365 14254.3810256 11926.3242383 9982.85122164 8370.85028611
)";

const std::string expected_hybrid_abundance =
    R"(year: 2003 2005 
catch[Line]: 5000 8000 
actual_catch[Line]: 5000 8000 
f_by_year[Line]: 0.0288463 0.0578775 
catch[Trawl]: 20000 40000 
actual_catch[Trawl]: 20000 40000 
f_by_year[Trawl]: 0.0445867 0.109472 
male: 74079.2218831 66641.6006324 58796.6068819 47963.0727573 38231.2372657 31255.8657556 24324.4405924 17753.7989801 11696.4613114 5828.95399056
female: 88895.0662597 74046.2229249 62471.394812 49333.4462646 38868.4245535 32506.1003859 26756.8846517 21896.3520754 18129.5150326 15155.2803755
)";

const std::string expected_hybrid_age_weight =
    R"(year: 2003 2005 
catch[Line]: 5000 8000 
actual_catch[Line]: 5000 8000 
f_by_year[Line]: 0.00433261 0.00814246 
catch[Trawl]: 20000 40000 
actual_catch[Trawl]: 20000 40000 
f_by_year[Trawl]: 0.00867402 0.0200208 
male: 74081.3774714 66667.8268106 59178.4485166 51110.961247 43226.8009014 35912.287203 28615.5777032 21377.8553786 14227.6337321 7110.43909038
female: 88897.6529656 74075.3631229 62877.1015488 52571.2744255 43947.2475831 37348.7786911 31477.1354735 26366.0216337 22052.8322848 18487.141635
)";

const std::string expected_instantaneous_biomass_u =
    R"(m_by_age[male]: 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 
m_by_age[female]: 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 
year: 2003 2005 
exploitation_rate[Line]: 0.544837 0.336939 
fishing_pressure[Line]: 0.551522 0.811775 
catch[Line]: 90000 30000 
actual_catch[Line]: 90000 25869.2 
exploitation_rate[Quota]: 0.1 0.2 
fishing_pressure[Quota]: 0.1 0.2 
catch[Quota]: 0.1 0.2 
actual_catch[Quota]: 9085.88 7056.98 
exploitation_rate[Trawl]: 0.0131853 0.36708 
fishing_pressure[Trawl]: 0.551522 0.811775 
catch[Trawl]: 4000 70000 
actual_catch[Trawl]: 4000 60361.5 
male: 60639.2561099 54454.7156768 46773.8322693 29106.1341794 16251.5691702 11048.4576004 5796.92031533 2555.92036984 1282.02360309 587.553602919
female: 72770.2896752 60555.414444 50451.4459024 35014.8982609 22539.2928946 15942.9759929 8855.93985449 4378.18479823 2759.91152081 2121.72134387
)";

const std::string expected_instantaneous_age_weight =
    R"(m_by_age[male]: 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 
m_by_age[female]: 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 0.1 
year: 2003 2005 
exploitation_rate[Line]: 0.0839952 0.0390204 
fishing_pressure[Line]: 0.084863 0.0824699 
catch[Line]: 90000 30000 
actual_catch[Line]: 90000 30000 
exploitation_rate[Quota]: 0.1 0.2 
fishing_pressure[Quota]: 0.1 0.2 
catch[Quota]: 0.1 0.2 
actual_catch[Quota]: 86815.8 121310 
exploitation_rate[Trawl]: 0.00186991 0.043915 
fishing_pressure[Trawl]: 0.084863 0.0824699 
catch[Trawl]: 4000 70000 
actual_catch[Trawl]: 4000 70000 
male: 60648.9211459 54531.3700519 47671.4163396 35418.144944 25359.580599 20376.7846581 15643.4213668 11259.9412741 7371.21666283 3666.8377159
female: 72781.8882255 60640.6566016 51419.6029246 42608.2946694 35171.1892445 29403.7955675 23898.4135035 19287.8089227 15868.5891127 13241.3584185
)";

class FisheryTableResults : public InternalEmptyModel {
protected:
  /**
   * Run the process and return everything it is responsible for: its report (which covers every
   * per-fishery catch map) and the final partition, to more digits than the report prints.
   */
  std::string RunAndDescribe(const std::string& process) {
    AddConfigurationLine(fishery_base_model, __FILE__, 41);
    AddConfigurationLine(process, __FILE__, 1);
    LoadConfiguration();
    model_->Start(RunMode::kBasic);

    ostringstream result;
    model_->managers()->process()->GetProcess("mort")->FillReportCache(result);
    result << std::setprecision(12);
    for (const char* label : {"male", "female"}) {
      result << label << ":";
      for (Double value : model_->partition().category(label).data_) result << " " << AS_DOUBLE(value);
      result << "\n";
    }
    return result.str();
  }
};

/**
 * Two fisheries, '*' expanding to one method row per category (so each fishery is seen twice), no
 * time_step_proportions (each step takes the whole M) and a capped, penalised year.
 */
TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Retained_TwoFisheries_TwoCategories) {
  EXPECT_EQ(expected_retained_two_fisheries, RunAndDescribe(retained_process));
}

TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Retained_AgeWeightColumn) {
  EXPECT_EQ(expected_retained_age_weight, RunAndDescribe(WithAgeWeightColumn(retained_process)));
}

/**
 * Retained is the one class whose method_<label> addressable is the retained catch. catches_ is
 * recalculated as total catch during DoExecute, so the two are different numbers after a run.
 */
TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Retained_MethodAddressable_IsRetainedCatch) {
  RunAndDescribe(retained_process);
  map<unsigned, Double>* catches = model_->managers()->process()->GetProcess("mort")->GetAddressableUMap("method_fishingpot");
  ASSERT_NE(nullptr, catches);
  EXPECT_DOUBLE_EQ(3000.0, AS_DOUBLE((*catches)[2003]));
  EXPECT_DOUBLE_EQ(90000.0, AS_DOUBLE((*catches)[2005]));
}

/**
 * Catches in numbers. Hybrid builds actual_catches_ with += during DoExecute, so it must start empty
 * rather than seeded with the observed catch.
 */
TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Hybrid_Abundance_SkipFTimeStep) {
  EXPECT_EQ(expected_hybrid_abundance, RunAndDescribe(hybrid_process));
}

TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Hybrid_AgeWeightColumn) {
  EXPECT_EQ(expected_hybrid_age_weight, RunAndDescribe(WithAgeWeightColumn(hybrid_process)));
}

/**
 * Per-fishery biomass and u flags: one biomass fishery, one in numbers, and one exploitation rate in
 * its own time step.
 */
TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Instantaneous_BiomassAndUColumns) {
  EXPECT_EQ(expected_instantaneous_biomass_u, RunAndDescribe(instantaneous_process));
}

TEST_F(FisheryTableResults, Processes_Mortality_FisheryBase_Discriminating_Instantaneous_AgeWeightColumn) {
  EXPECT_EQ(expected_instantaneous_age_weight, RunAndDescribe(WithAgeWeightColumn(instantaneous_process)));
}

/**
 * One way of breaking a table: applied to process, this must fail with a message containing expected
 */
struct FisheryErrorCase {
  std::string name;
  std::string process;
  std::string old_text;
  std::string new_text;
  std::string expected;
};

std::vector<FisheryErrorCase> GetFisheryErrorCases() {
  const std::string table_missing_column = "is missing the following column headers";

  struct ProcessTable {
    std::string prefix;
    std::string process;
    std::string catch_row;
    std::string bad_year_row;
    std::string old_year_row;
    std::string bad_catch_row;
    std::string negative_catch_row;
    std::string last_fishery_row_start;
    std::string ghost_fishery_row_start;
    std::string selectivity_cell;
  };
  const std::vector<ProcessTable> tables = {
      {"Instantaneous", instantaneous_process, "2003 4000  90000 0.1", "abcd 4000  90000 0.1", "1999 4000  90000 0.1", "2003 abcd  90000 0.1", "2003 -4000 90000 0.1", "Line   *", "Ghost  *", "*        fishSel"},
      {"Retained", retained_process, "2003 3000       1000", "abcd 3000       1000", "1999 3000       1000", "2003 abcd       1000", "2003 -3000      1000", "FishingLine *", "Ghost       *", "*        fishSel"},
      {"Hybrid", hybrid_process, "2003 20000 5000", "abcd 20000 5000", "1999 20000 5000", "2003 abcd  5000", "2003 -2000 5000", "Line   *", "Ghost  *", "*        fishSel"}};

  std::vector<FisheryErrorCase> cases;
  for (const ProcessTable& table : tables) {
    const std::string& p = table.prefix;
    cases.push_back({p + "_YearNotANumber", table.process, table.catch_row, table.bad_year_row, "could not be converted to an unsigned integer"});
    cases.push_back({p + "_YearNotInModel", table.process, table.catch_row, table.old_year_row, "not a valid year"});
    cases.push_back({p + "_CatchNotANumber", table.process, table.catch_row, table.bad_catch_row, "could not be converted to a Double"});
    cases.push_back({p + "_NegativeCatch", table.process, table.catch_row, table.negative_catch_row, "must be >= 0"});
    cases.push_back({p + "_FisheryWithoutCatches", table.process, table.last_fishery_row_start, table.ghost_fishery_row_start, "does not have catch information"});
    cases.push_back({p + "_SelectivityCount", table.process, table.selectivity_cell, "*        fishSel,lineSel", "and selectivities (2) provided must be the same"});
    cases.push_back({p + "_UnknownSelectivity", table.process, table.selectivity_cell, "*        missingSel", "missingSel"});
    cases.push_back({p + "_UnknownTimeStep", table.process, "step1 ", "stepX ", "Time step label stepX was not found"});
    cases.push_back({p + "_UnknownMOgive", table.process, "relative_m_by_age One", "relative_m_by_age Missing", "M-by-age ogive label Missing was not found"});
    cases.push_back({p + "_UnknownAgeWeight", WithAgeWeightColumn(table.process, "missing"), "type mortality", "type mortality", "age weight label missing was not found"});
    cases.push_back({p + "_AgeWeightCount", WithAgeWeightColumn(table.process), " weights\n", " weights,weights\n", "and age weights (2) provided must be the same"});
  }

  // Category is valid for the model but is not one of the process's categories
  cases.push_back({"Instantaneous_CategoryNotInProcess", Replaced(instantaneous_process, "categories *\n", "categories male\n"), "Trawl  *", "Trawl  female",
                   "was found in the methods table but not in the 'categories' subcommand"});
  cases.push_back({"Retained_CategoryNotInProcess", Replaced(retained_process, "categories *\n", "categories male\n"), "FishingPot  *", "FishingPot  female",
                   "was found in the methods table but not in the 'categories' subcommand"});
  cases.push_back({"Hybrid_CategoryNotInProcess", Replaced(hybrid_process, "categories *\n", "categories male\n"), "Trawl  *", "Trawl  female",
                   "was found in the methods table but not in the 'categories' subcommand"});

  // Two rows of one fishery that disagree
  const std::string inst_row = "Trawl  *        fishSel     0.7   step1     CatchPenalty true    false\n";
  const auto        inst_pair = [](const std::string& female_rest) {
    return "Trawl  male     fishSel     0.7   step1     CatchPenalty true    false\nTrawl  female   fishSel     " + female_rest + "\n";
  };
  cases.push_back({"Instantaneous_RepeatedTimeStep", instantaneous_process, inst_row, inst_pair("0.7   step2     CatchPenalty true    false"), "had timestep label = step2"});
  cases.push_back({"Instantaneous_RepeatedPenalty", instantaneous_process, inst_row, inst_pair("0.7   step1     none         true    false"), "had penalty = none"});
  cases.push_back({"Instantaneous_RepeatedUMax", instantaneous_process, inst_row, inst_pair("0.5   step1     CatchPenalty true    false"), "had u_max = 0.5"});
  cases.push_back({"Instantaneous_RepeatedBiomass", instantaneous_process, inst_row, inst_pair("0.7   step1     CatchPenalty false   false"), "had biomass = 0"});
  cases.push_back({"Instantaneous_RepeatedU", instantaneous_process, inst_row, inst_pair("0.7   step1     CatchPenalty true    true"), "had U = 1"});

  const std::string ret_row = "FishingPot  *        fishSel     retSel               fifty_percent                 0.7   step1     CatchPenalty\n";
  const auto        ret_pair = [](const std::string& female_rest) {
    return "FishingPot  male     fishSel     retSel               fifty_percent                 0.7   step1     CatchPenalty\n"
           "FishingPot  female   fishSel     retSel               fifty_percent                 "
           + female_rest + "\n";
  };
  cases.push_back({"Retained_RepeatedTimeStep", retained_process, ret_row, ret_pair("0.7   step2     CatchPenalty"), "had timestep label = step2"});
  cases.push_back({"Retained_RepeatedPenalty", retained_process, ret_row, ret_pair("0.7   step1     none"), "had penalty = none"});
  cases.push_back({"Retained_RepeatedUMax", retained_process, ret_row, ret_pair("0.5   step1     CatchPenalty"), "had u_max = 0.5"});

  const std::string hyb_row  = "Trawl  *        fishSel     1               step1     CatchPenalty\n";
  const auto        hyb_pair = [](const std::string& female_rest) {
    return "Trawl  male     fishSel     1               step1     CatchPenalty\nTrawl  female   fishSel     " + female_rest + "\n";
  };
  cases.push_back({"Hybrid_RepeatedTimeStep", hybrid_process, hyb_row, hyb_pair("1               step2     CatchPenalty"), "had timestep label = step2"});
  cases.push_back({"Hybrid_RepeatedPenalty", hybrid_process, hyb_row, hyb_pair("1               step1     none"), "had penalty = none"});

  // Required columns: mortality_instantaneous checks its own as Age models have optional columns
  const std::string inst_header = "method category selectivity u_max time_step penalty      biomass u";
  cases.push_back({"Instantaneous_MissingMethodColumn", instantaneous_process, inst_header, "wrong category selectivity u_max time_step penalty biomass u", "required column method"});
  cases.push_back({"Instantaneous_MissingCategoryColumn", instantaneous_process, inst_header, "method wrong selectivity u_max time_step penalty biomass u", "required column category"});
  cases.push_back({"Instantaneous_MissingSelectivityColumn", instantaneous_process, inst_header, "method category wrong u_max time_step penalty biomass u", "required column selectivity"});
  cases.push_back({"Instantaneous_MissingTimeStepColumn", instantaneous_process, inst_header, "method category selectivity u_max wrong penalty biomass u", "required column time_step"});
  cases.push_back({"Instantaneous_MissingUMaxColumn", instantaneous_process, inst_header, "method category selectivity wrong time_step penalty biomass u", "required column u_max"});
  cases.push_back({"Instantaneous_MissingPenaltyColumn", instantaneous_process, inst_header, "method category selectivity u_max time_step wrong biomass u", "required column penalty"});
  const std::string ret_header = "retained_selectivity discard_mortality_selectivity u_max";
  cases.push_back({"Retained_MissingRetainedColumn", retained_process, ret_header, "wrong discard_mortality_selectivity u_max", table_missing_column});
  cases.push_back({"Retained_MissingDiscardColumn", retained_process, ret_header, "retained_selectivity wrong u_max", table_missing_column});
  cases.push_back({"Retained_MissingUMaxColumn", retained_process, ret_header, "retained_selectivity discard_mortality_selectivity wrong", table_missing_column});
  cases.push_back({"Hybrid_MissingAnnualDurationColumn", hybrid_process, "annual_duration time_step", "wrong time_step", table_missing_column});

  // mortality_instantaneous columns
  cases.push_back({"Instantaneous_BadUMax", instantaneous_process, "fishSel     0.7", "fishSel     abc", "u_max value abc could not be converted"});
  cases.push_back({"Instantaneous_BadBiomassFlag", instantaneous_process, "CatchPenalty true ", "CatchPenalty maybe ", "unable to convert 'maybe'"});
  cases.push_back({"Instantaneous_BadUFlag", instantaneous_process, "CatchPenalty true    false", "CatchPenalty true    maybe", "unable to convert 'maybe'"});
  cases.push_back({"Instantaneous_ExploitationRateAboveOne", instantaneous_process, "90000 0.1", "90000 1.5", "It must have a value between 0 and 1"});
  cases.push_back({"Instantaneous_UnknownPenalty", instantaneous_process, "CatchPenalty", "MissingPenalty", "MissingPenalty"});
  cases.push_back({"Instantaneous_TimeStepProportionsLength", instantaneous_process, "time_step_proportions 0.6 0.4", "time_step_proportions 1.0", "time step proportions length (1)"});
  cases.push_back({"Instantaneous_TimeStepProportionsSum", instantaneous_process, "time_step_proportions 0.6 0.4", "time_step_proportions 0.6 0.3", "do not sum to one"});

  // mortality_instantaneous_retained columns
  cases.push_back({"Retained_BadUMax", retained_process, "fifty_percent                 0.7", "fifty_percent                 abc", "u_max value abc could not be converted"});
  cases.push_back({"Retained_RetainedSelectivityCount", retained_process, "fishSel     retSel  ", "fishSel     retSel,retSel  ", "and retained selectivities (2) provided must be the same"});
  cases.push_back({"Retained_DiscardSelectivityCount", retained_process, "retSel               fifty_percent", "retSel               fifty_percent,One", "and discard mortality selectivities (2) provided must be the same"});
  cases.push_back({"Retained_UnknownRetainedSelectivity", retained_process, "fishSel     retSel", "fishSel     missingSel", "missingSel"});
  cases.push_back({"Retained_UnknownDiscardSelectivity", retained_process, "retSel               fifty_percent", "retSel               missingSel", "missingSel"});
  cases.push_back({"Retained_TimeStepProportionsLength", retained_process, "relative_m_by_age One\n", "relative_m_by_age One\ntime_step_proportions 1.0\n", "time step proportions length (1)"});
  cases.push_back({"Retained_TimeStepProportionsSum", retained_process, "relative_m_by_age One\n", "relative_m_by_age One\ntime_step_proportions 0.6 0.3\n", "do not sum to one"});

  // mortality_hybrid columns
  cases.push_back({"Hybrid_BadAnnualDuration", hybrid_process, "fishSel     1 ", "fishSel     abc ", "annual_duration value abc could not be converted"});
  cases.push_back({"Hybrid_AnnualDurationAboveOne", hybrid_process, "fishSel     1 ", "fishSel     1.5 ", "This must be a value between 0 and 1"});
  cases.push_back({"Hybrid_AgeWeightNeedsBiomass", WithAgeWeightColumn(hybrid_process), "biomass true\n", "biomass false\n", "must be true if an Age weight relationship is also used"});
  cases.push_back({"Hybrid_TimeStepProportionsLength", hybrid_process, "time_step_proportions 0.6 0.4", "time_step_proportions 1.0", "time step proportions length (1)"});
  cases.push_back({"Hybrid_TimeStepProportionsSum", hybrid_process, "time_step_proportions 0.6 0.4", "time_step_proportions 0.6 0.3", "needs to sum to one"});

  return cases;
}

class FisheryTableErrors : public InternalEmptyModel, public ::testing::WithParamInterface<FisheryErrorCase> {};

TEST_P(FisheryTableErrors, Fails) {
  const FisheryErrorCase& error_case = GetParam();
  AddConfigurationLine(fishery_base_model, __FILE__, 41);
  AddConfigurationLine(Replaced(error_case.process, error_case.old_text, error_case.new_text), __FILE__, 1);

  try {
    LoadConfiguration();
    model_->Start(RunMode::kBasic);
    FAIL() << "expected an error containing: " << error_case.expected;
  } catch (const std::string& message) {
    EXPECT_NE(std::string::npos, message.find(error_case.expected)) << message;
  }
}

INSTANTIATE_TEST_CASE_P(Processes_Mortality_FisheryBase, FisheryTableErrors, ::testing::ValuesIn(GetFisheryErrorCases()),
                        [](const ::testing::TestParamInfo<FisheryErrorCase>& info) { return info.param.name; });

/**
 * mortality_instantaneous is the only child with a Length profile
 */
const std::string fishery_length_model =
    R"(
@model
type length
start_year 2001
final_year 2005
length_bins 1:15
length_plus true
length_plus_group 15
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Annual

@categories
format sex
names male female
growth_increment growth_model growth_model

@initialisation_phase Equilibrium_state
type iterative
years 10

@time_step Annual
processes mortality

@process mortality
type mortality_instantaneous
time_step_proportions 1
m 0.0798
relative_m_by_length One
categories male
table catches
year Fishing
2003 100
end_table
table method
method  category selectivity u_max time_step penalty
Fishing male     One         0.7   Annual    none
end_table

@selectivity One
type constant
c 1

@length_weight allometric
type basic
a 0.000000000373
b 3.145
units tonnes

@growth_increment growth_model
type basic
l_alpha 5
l_beta  10
g_alpha 7
g_beta 1
min_sigma 2
distribution normal
length_weight allometric
cv 0.0
compatibility_option casal
)";

class FisheryTableLengthErrors : public InternalEmptyLengthModel {
protected:
  void ExpectError(const std::string& config, const std::string& expected) {
    AddConfigurationLine(config, __FILE__, 1);
    try {
      LoadConfiguration();
      model_->Start(RunMode::kBasic);
      FAIL() << "expected an error containing: " << expected;
    } catch (const std::string& message) {
      EXPECT_NE(std::string::npos, message.find(expected)) << message;
    }
  }
};

TEST_F(FisheryTableLengthErrors, Processes_Mortality_FisheryBase_Length_CategoryNotInProcess) {
  ExpectError(Replaced(fishery_length_model, "Fishing male", "Fishing female"), "was found in the methods table but not in the 'categories' subcommand");
}

TEST_F(FisheryTableLengthErrors, Processes_Mortality_FisheryBase_Length_UnknownMOgive) {
  ExpectError(Replaced(fishery_length_model, "relative_m_by_length One", "relative_m_by_length Missing"), "M-by-length ogive label Missing was not found");
}

} /* namespace common */
} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */
