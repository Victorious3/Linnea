#pragma once

#include <string>
#include <map>
#include <fstream>
#include <exception>
#include <vector>

using namespace std;

class Configuration;

class ConfigurationGroup
{
public:
	ConfigurationGroup(Configuration* parent, string name, string comment = "");
	~ConfigurationGroup();

	double& getReal(string key, double def = 0, string comment = "");
	double& getReal(string key, double def, double min, double max, string comment = "");

	long& getInteger(string key, long def = 0, string comment = "");
	long& getInteger(string key, long def, long min, long max, string comment = "");

	bool& getBool(string key, bool def = false, string comment = "");

	string& getString(string key, string def = "", string comment = "");

	/// The default parameter is assumed to be part of the allowed string literals
	string& getString(string key, string def, vector<string>& allowed, string comment = "");

	void save(ofstream& file);

	// Entry structs
	struct Entry {
		virtual ~Entry() {};
		string comment;
		virtual string to_string();
	};

	struct RealEntry : Entry { 
		RealEntry(string _comment, double _value);
		RealEntry(string _comment);

		virtual string to_string() override;
		union {
			double d;
			long l;
			bool b;
		} value;
	};

	struct IntegerEntry : RealEntry {
		IntegerEntry(string _comment);
		IntegerEntry(string _comment, long _value);

		virtual string to_string() override;
	};

	struct BoolEntry : IntegerEntry {
		BoolEntry(string _comment, bool _value);
	};
	
	struct StringEntry : Entry {
		StringEntry(string _comment, string _value);
		string value;

		virtual string to_string() override;
	};

	map<string, Entry*> _entries;
	string _comment;

protected:
	Configuration* _parent = nullptr;
	const string _name;
};

class Configuration : public ConfigurationGroup
{
public:
	Configuration(string file);
	~Configuration();
	
	ConfigurationGroup* getGroup(string key, string comment = "");

	void save();

	string file();

	bool _changed = false;

private:

	void clear();

	void load();

	string _file;

	map<string, ConfigurationGroup*> _groups;
};



