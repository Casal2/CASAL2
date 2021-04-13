/**
 * @file Accessor.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 14/01/2020
 * @section LICENSE
 *
 * Copyright Scott Rasmussen (Zaita) �2020 - https://www.zaita.com
 *
 * @description
 * This class is a unified accessor for our partition. It has been designed to replace
 * the 8+ current acessors that exist in the code base.
 *
 * In general, the accessor:
 * - Can be iterated over using standard C++ for loops
 * - Supports one, many or all categories
 * - Supports caching of values so they can be interpolated
 *
 */
#ifndef SOURCE_PARTITION_AACCESSOR_H_
#define SOURCE_PARTITION_AACCESSOR_H_

// headers
#include <vector>

#include "Partition.h"
#include "../Utilities/Types.h"

// namespaces
namespace niwa {
class Model;
namespace partition {

// forward declarations
using niwa::utilities::Double;

/**
 * Create a special iterate object so we can make it more intuitive when
 * accessing it within a process. The process will be able to do
 * for (auto category : accessor) {
 *  Double x = category.data_;
 *  Double y = category.cached_data_;
 * }
 */
class PartitionIterable  {
public:
	PartitionIterable() = delete;
	virtual ~PartitionIterable() = default;
	// have override constructor so we can build the data_ reference nicely
	// avoids pointer semantics and a deference on every access
	PartitionIterable(partition::Category* category, vector<Double>& category_data) :
		category_(category), data_(category_data) { }

	partition::Category* category_;
	vector<Double>& data_;
	vector<Double>  cached_data_;
};

/**
 * The accessor class definition. This holds a list of
 * PartitionIterable and provides convenient access methods
 */
class Accessor {
public:
	// Typedefs
	typedef vector<PartitionIterable> DataType;

	// methods
	explicit Accessor(shared_ptr<Model> model);
	virtual ~Accessor() = default;
	void													Init(const vector<string>& category_labels);
	Accessor::DataType::iterator  begin();
	Accessor::DataType::iterator  end();

protected:
  // members
  shared_ptr<Model>									model_;
  map<unsigned, Accessor::DataType>	categories_;
};


} /* namespace niwa::partition */
}
#endif /* SOURCE_PARTITION_AACCESSOR_H_ */
