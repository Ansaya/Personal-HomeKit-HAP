#ifndef HAP_DEFINES
#define HAP_DEFINES

#include <memory>

namespace hap {

	class AccessorySet;

	class Accessory;
	typedef std::shared_ptr<Accessory> Accessory_ptr;

	class Service;
	typedef std::shared_ptr<Service> Service_ptr;

	class Characteristics;
	typedef std::shared_ptr<Characteristics> Characteristics_ptr;

	class BoolCharacteristics;
	typedef std::shared_ptr<BoolCharacteristics> BoolCharacteristics_ptr;

	class IntCharacteristics;
	typedef std::shared_ptr<IntCharacteristics> IntCharacteristics_ptr;

	class FloatCharacteristics;
	typedef std::shared_ptr<FloatCharacteristics> FloatCharacteristics_ptr;

	class StringCharacteristics;
	typedef std::shared_ptr<StringCharacteristics> StringCharacteristics_ptr;

}

#endif