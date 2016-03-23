#include "Configuration.h"

#include <sstream>
#include "common.h"

#pragma region Entry Structs

string ConfigurationGroup::Entry::to_string() { return ""; }

ConfigurationGroup::StringEntry::StringEntry(string _comment, string _value) {
	comment = _comment; value = _value;
}

string ConfigurationGroup::StringEntry::to_string() {
	char first = value.at(0);
	if (first == '\'' || first == '"' || first == ' ' || value.at(value.length() - 1) == ' ') {
		// We need to encapsulate with quotes
		return "\"" + value + "\"";
	}

	return value;
}

ConfigurationGroup::RealEntry::RealEntry(string _comment) {
	comment = _comment;
}

ConfigurationGroup::RealEntry::RealEntry(string _comment, double _value) {
	comment = _comment; value.d = _value;
}

string ConfigurationGroup::RealEntry::to_string() { return std::to_string(value.d); }

ConfigurationGroup::IntegerEntry::IntegerEntry(string _comment, long _value) : RealEntry(_comment) {
	value.l = _value;
}

ConfigurationGroup::IntegerEntry::IntegerEntry(string _comment) : RealEntry(_comment) {}

string ConfigurationGroup::IntegerEntry::to_string() { return std::to_string(value.l); }

ConfigurationGroup::BoolEntry::BoolEntry(string _comment, bool _value) : IntegerEntry(_comment) {
	value.b = _value;
}

#pragma endregion

ConfigurationGroup::ConfigurationGroup(Configuration* parent, string name, string comment) : _parent(parent), _name(name), _comment(comment) {}

ConfigurationGroup::~ConfigurationGroup()
{
	for (auto it = _entries.begin(); it != _entries.end(); it++) {
		delete it->second;
	}
	_entries.clear();
}

#pragma region ConfigurationGroup Accessors

double& ConfigurationGroup::getReal(string key, double def, string comment) {
	auto it = _entries.find(key);
	if (it != _entries.end()) {
		Entry* entry = it->second;
		RealEntry* real = dynamic_cast<RealEntry*>(entry);
		if (real != nullptr) {
			if (!comment.empty()) {
				real->comment = comment;
				_parent->_changed = true;
			}
			return real->value.d;
		}
		LOG->warn(" [Config] \"{}\"/{}:{} not a real number, setting back to default value {}", _parent->file(), _name, key, def);
	}
	RealEntry* real = new RealEntry(comment, def);
	_entries[key] = real;
	_parent->_changed = true;
	return real->value.d;
}

double& ConfigurationGroup::getReal(string key, double def, double min, double max, string comment) {
	if (def < min || def > max) throw "Invalid property, default value out of specified range";
	double& real = getReal(key, def, comment);
	if (real < min || real > max) {
		LOG->warn(" [Config] \"{}\"/{}:{} out of range ({},{}), setting back to default value {}", _parent->file(), _name, key, min, max, def);
		RealEntry* real = new RealEntry(comment, def);
		_entries[key] = real;
		_parent->_changed = true;
		return real->value.d;
	}
	return real;
}

long& ConfigurationGroup::getInteger(string key, long def, string comment) {
	auto it = _entries.find(key);
	if (it != _entries.end()) {
		Entry* entry = it->second;
		IntegerEntry* integer = dynamic_cast<IntegerEntry*>(entry);
		if (integer != nullptr) {
			if (!comment.empty()) {
				integer->comment = comment;
				_parent->_changed = true;
			}
			return integer->value.l;
		}
		LOG->warn(" [Config] \"{}\"/{}:{} not an integer, setting back to default value {}", _parent->file(), _name, key, def);
	}
	IntegerEntry* integer = new IntegerEntry(comment, def);;
	_entries[key] = integer;
	_parent->_changed = true;
	return integer->value.l;
}

long& ConfigurationGroup::getInteger(string key, long def, long min, long max, string comment) {
	if (def < min || def > max) throw "Invalid property, default value out of specified range";
	long& integer = getInteger(key, def, comment);
	if (integer < min || integer > max) {
		LOG->warn(" [Config] \"{}\"/{}:{} out of range ({},{}), setting back to default value {}", _parent->file(), _name, key, min, max, def);
		IntegerEntry* integer = new IntegerEntry(comment, def);
		_entries[key] = integer;
		_parent->_changed = true;
		return integer->value.l;
	}
	return integer;
}

bool& ConfigurationGroup::getBool(string key, bool def, string comment) {
	auto it = _entries.find(key);
	if (it != _entries.end()) {
		Entry* entry = it->second;
		BoolEntry* boolean = dynamic_cast<BoolEntry*>(entry);
		if (boolean != nullptr) {
			if (!comment.empty()) {
				boolean->comment = comment;
				_parent->_changed = true;
			}
			return boolean->value.b;
		}
		LOG->warn(" [Config] \"{}\"/{}:{} not a boolean, setting back to default value {}", _parent->file(), _name, key, def);
	}
	BoolEntry* boolean = new BoolEntry(comment, def);
	_entries[key] = boolean;
	_parent->_changed = true;
	return boolean->value.b;
}

string& ConfigurationGroup::getString(string key, string def, string comment) {
	auto it = _entries.find(key);
	if (it != _entries.end()) {
		Entry* entry = it->second;
		StringEntry* str = dynamic_cast<StringEntry*>(entry);
		if (str != nullptr) {
			if (!comment.empty()) {
				str->comment = comment;
				_parent->_changed = true;
			}
			return str->value;
		}
		LOG->warn(" [Config] \"{}\"/{}:{} not a string, setting back to default value {}", _parent->file(), _name, key, def);
	}
	StringEntry* entry = new StringEntry(comment, def);
	_entries[key] = entry;
	_parent->_changed = true;
	return entry->value;
}

string& ConfigurationGroup::getString(string key, string def, vector<string>& allowed, string comment) {
	string& str = getString(key, def, comment);
	if (str == def) return str;
	if (find(allowed.begin(), allowed.end(), str) != allowed.end()) {
		return str;
	}
	string members;
	for (uint32 i = 0; i < allowed.size(); i++) {
		if (i != 0) members += ", ";
		members += allowed[i];
	}
	LOG->warn(" [Config] \"{}\"/{}:{} not a member of [{}], setting back to default value {}", _parent->file(), _name, key, members, def);
	StringEntry* entry = new StringEntry(comment, def);
	_entries[key] = entry;
	_parent->_changed = true;
	return entry->value;
}

#pragma endregion

Configuration::Configuration(string file) : ConfigurationGroup(this, ""), _file(file)
{
	load();
}

Configuration::~Configuration()
{
	clear();
}

ConfigurationGroup* Configuration::getGroup(string key, string comment) {
	auto it = _groups.find(key);
	if (it != _groups.end()) {
		if (!comment.empty()) {
			it->second->_comment = comment;
			_changed = true;
		}
		return it->second;
	}
	ConfigurationGroup* group = new ConfigurationGroup(this, key, comment);
	_groups.emplace(key, group);
	_changed = true;
	return group;
}

void Configuration::clear() {
	for (auto it = _groups.begin(); it != _groups.end(); it++) {
		delete it->second;
	}
	_groups.clear();
	_entries.clear();
}

void Configuration::load() {
	ifstream ifs;
	ifs.open(_file);

	if (ifs.fail()) {
		LOG->error(" [Config] Couldn't open configuration file \"{}\", falling back to default configuration. {}", _file, common::syserror());

		clear();
		_changed = true;
		return;
	}
	
	ConfigurationGroup* currentGroup = this;

	int ln = 0;
	string line;
	while (getline(ifs, line)) {
		common::str_trim(line);
		// Don't need empty lines
		if (line.empty()) continue;
		// Ignore comments
		if (line.at(0) == ';') continue;

		if (line.at(0) == '[') {
			string name = line.substr(1, line.length() - 2);
			if (line.at(line.length() - 1) != ']') {
				// invalid group
				goto error;
			}
			// Create a new group and add it to the configuration
			ConfigurationGroup* group = new ConfigurationGroup(this, name);
			_groups.emplace(name, group);
			currentGroup = group;
		} else {
			uint32 ieq = line.find_first_of('=');
			if (ieq == string::npos) goto error;

			string key = line.substr(0, ieq);
			string value = line.substr(ieq + 1, line.length() - 1);

			// Needs a key
			if (key.empty()) goto error;

			bool quoted = false;
			if (!value.empty()) {
				char qc = value.at(0);
				// Get rid of quotation marks
				if (qc = '"' || qc == '\'') {
					if (value.at(value.length() - 1) == qc) {
						value = value.substr(1, value.length() - 2);
						quoted = true;
					}
				}
			}

			if (!quoted) {
				if (value.length() == 1) {
					// Boolean entry, 0 or 1
					if (value.at(0) == '0' || value.at(0) == '1') {
						currentGroup->_entries[key] = new BoolEntry { "", value.at(0) == '1' };
						continue;
					}
				}
				// Parse number
				char *err;
				double n = strtod(value.c_str(), &err);
				if (*err == 0 && !value.empty()) {
					double intpart;
					if (modf(n, &intpart) == 0.0) {
						// We have an integer
						currentGroup->_entries[key] = new IntegerEntry { "", (long) n };
					} else {
						// We have a real number
						currentGroup->_entries[key] = new RealEntry { "", n };
					}
					continue;
				}
			}

			// If it wasn't a number or a boolean it has to be a string
			currentGroup->_entries[key] = new StringEntry { "", value };
		}
		ln++;
	}
	ifs.close();
	return;

error:
	LOG->error(" [Config] Couldn't parse configuration file \"{}\", error on line {}. Falling back to default configuration.", _file, ln);
	clear();
	_changed = true;
	ifs.close();
}

inline void spawnComment(ofstream& file, string comment) {
	if (comment.empty()) return;
	file << endl;
	stringstream ss(comment);
	for (std::string line; getline(ss, line); ) {
		file << ';' << line << endl;
	}
}

void ConfigurationGroup::save(ofstream& file) {
	spawnComment(file, _comment);
	if (!_name.empty()) file << "[" << _name << "]" << endl;
	for (auto it = _entries.begin(); it != _entries.end(); it++) {
		spawnComment(file, it->second->comment);
		file << it->first << "=" << it->second->to_string() << endl;
	}
}

void Configuration::save() {
	// We don't need to save anything if the configuration hasn't changed.
	if (!_changed) {
		if (common::file_exists(_file)) return;
	}

	ofstream f(_file);
	if (f.fail()) {
		LOG->error(" [Config] Couldn't save configuration file \"{}\". {}", _file, common::syserror());
		return;
	}

	ConfigurationGroup::save(f);
	for (auto it = _groups.begin(); it != _groups.end(); it++) {
		it->second->save(f);
		f << endl;
	}
}

string Configuration::file()
{
	return _file;
}
