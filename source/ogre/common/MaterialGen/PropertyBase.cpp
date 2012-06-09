#include "PropertyBase.hpp"

#include <boost/lexical_cast.hpp>

namespace sh
{
	StringValue::StringValue (const std::string& in)
	{
		mValue = in;
	}

	void StringValue::deserialize (const std::string& in)
	{
		mValue = in;
	}

	std::string StringValue::serialize ()
	{
		return mValue;
	}

	// ------------------------------------------------------------------------------

	IntValue::IntValue (int in)
	{
		mValue = in;
	}

	void IntValue::deserialize (const std::string& in)
	{
		mValue = boost::lexical_cast<int>(in);
	}

	std::string IntValue::serialize ()
	{
		return boost::lexical_cast<std::string>(mValue);
	}

	// ------------------------------------------------------------------------------

	FloatValue::FloatValue (float in)
	{
		mValue = in;
	}

	void FloatValue::deserialize (const std::string& in)
	{
		mValue = boost::lexical_cast<float>(in);
	}

	std::string FloatValue::serialize ()
	{
		return boost::lexical_cast<std::string>(mValue);
	}

	// ------------------------------------------------------------------------------

	void PropertySet::setProperty (const std::string& name, PropertyPtr value)
	{
		setPropertyOverride (name, value);
	}

	bool PropertySet::setPropertyOverride (const std::string& name, PropertyPtr value)
	{
		// if we got here, none of the sub-classes was able to make use of the property
		std::cerr << "sh::PropertySet: Warning: No match for property with name '" << name << "'" << std::endl;
		return false;
	}

	// ------------------------------------------------------------------------------

	void PropertySetGet::setProperty (const std::string& name, PropertyPtr value)
	{
		mProperties [name] = value;
	}

	PropertyPtr PropertySetGet::getProperty(const std::string& name)
	{
		assert (mProperties.find(name) != mProperties.end()
			&& "Trying to retrieve property that does not exist");
		return mProperties[name];
	}
}