//
// ValTree: load data from minimal text files
//
// ValTree is a file format for storing key value pairs
// in a minimal textual hierarchy and a C++ class to read
// and write those files. Data is queried and returned
// as guaranteed references. A query miss even returns a 
// a reference to a static blank object.
//
// ValTree is on GitHub:
// https://github.com/natweiss/valtree
//

#include "ValTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <string.h>
#include <stdlib.h>

const int   kMaxFileSize      = 8 * 1024 * 1024;
const char* kCommentStartTag  = "//";

using namespace std;

//
// static helper functions
//

static bool _isWhitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static int findWhitespace(const string& s, int start)
{
	unsigned i = start;
	for (; i < s.size() && !_isWhitespace(s[i]); i++)
		;
	return i;
}

static int findNonWhitespace(const string& s, int start)
{
	unsigned i = start;
	for (; i < s.size() && _isWhitespace(s[i]) && s[i] != '\n' && s[i] != '\r'; i++)
		;
	return i;
}

static int findNewline(const string& s, int start)
{
	unsigned i = start;
	for (; i < s.size() && s[i] != '\n' && s[i] != '\r'; i++)
		;
	return i;
}

static int findAfterNewline(const string& s, int start)
{
	unsigned i = findNewline(s, start);
	for (; i < s.size() && (s[i] == '\n' || s[i] == '\r'); i++)
		;
	return i;
}

static int getDepth(const string& data, int pos)
{
	for (unsigned i = pos; i < data.size(); i++)
		if (!_isWhitespace(data[i]))
			return i - pos;
	return -1;
}

static void _log(ostream& ss, const ValTree& v, int depth)
{
	if (v.getKey().size() > 0 || v.getStr().size() > 0)
	{
		for (int i = 0; i < depth; i++)
			ss << "\t";
		ss << v.getKey() << "  " << v.getStr() << endl;
	}

	for (int i = 0; i < v.size(); i++)
	{
		auto child = v.getIndex(i);
		_log(ss, child, depth + 1);
	}

	// note that a c++98 for loop is not any faster for saving large files
	//for (auto it = v.begin(), end = v.end(); it != end; ++it)
}

#pragma mark -
#pragma mark ValTree

//
// ValTree
//

ValTree::ValTree()
{
	valInt = 0;
	valFloat = 0.0;
}

ValTree::ValTree(const string& k, const string& v)
{
	this->set(k, v);
}

ValTree::ValTree(const ValTree& rhs)
{
	*this = rhs;
}

ValTree::~ValTree()
{
}

void ValTree::clear()
{
	key.clear();
	key.shrink_to_fit();
	val.clear();
	val.shrink_to_fit();
	valInt = 0;
	valFloat = 0.0;
	children.clear();
	children.shrink_to_fit();
}

bool ValTree::isNull() const
{
	return (key.size() <= 0 && val.size() <= 0 && children.size() == 0);
}

void ValTree::setValInt()
{
	valInt = strtoll(val.c_str(), nullptr, 10);
}

void ValTree::setValFloat()
{
	valFloat = strtod(val.c_str(), nullptr);
}

ValTree& ValTree::null()
{
	static ValTree blank;
	if (!blank.isNull())
		blank.clear();
	return blank;
}

void ValTree::set(const string& k, const string& v)
{
	key = k;
	val = v;
	this->setValInt();
	this->setValFloat();
}

#pragma mark -
#pragma mark Operators

ValTree& ValTree::operator=(const ValTree& rhs)
{
	key = rhs.key;
	val = rhs.val;
	valInt = rhs.valInt;
	valFloat = rhs.valFloat;
	children = rhs.children;
	return *this;
}

ValTree& ValTree::operator=(const string& rhs)
{
	if (!this->isNull())
	{
		val = rhs;
		this->setValInt();
		this->setValFloat();
	}
	return *this;
}

ValTree& ValTree::operator=(int rhs)
{
	if (!this->isNull())
	{
		valInt = rhs;
		this->setValFloat();
		stringstream ss;
		ss << rhs;
		val = ss.str();
	}
	return *this;
}

ValTree& ValTree::operator=(double rhs)
{
	if (!this->isNull())
	{
		valFloat = rhs;
		this->setValInt();
		stringstream ss;
		ss << rhs;
		val = ss.str();
	}
	return *this;
}

bool ValTree::operator==(const ValTree& rhs)
{
	return val == rhs.val;
}

bool ValTree::operator!=(const ValTree& rhs)
{
	return val != rhs.val;
}

bool ValTree::operator<(const ValTree& rhs)
{
	return val < rhs.val;
}

bool ValTree::operator>(const ValTree& rhs)
{
	return val > rhs.val;
}

#pragma mark -
#pragma mark Keys and Values

const string& ValTree::getKey() const
{
	return key;
}

const string& ValTree::getStr() const
{
	return val;
}

long ValTree::getInt() const
{
	return valInt;
}

double ValTree::getFloat() const
{
	return valFloat;
}

const vector<string> ValTree::getStrs(char delim) const
{
	vector<string> v;
	if (val.size() > 0)
	{
		string s;
		stringstream ss(val);
		while(std::getline(ss, s, delim))
			if (!s.empty())
				v.push_back(s);
	}
	return v;
}

vector<long> ValTree::getInts(char delim) const
{
	vector<long> v;
	if (!this->isNull())
	{
		size_t pos = 0, lastPos = 0;
		bool good = false;
		do
		{
			pos = val.find(delim, pos);
			good = (pos != string::npos);
			if (good || lastPos > 0)
				v.push_back(strtol(&val[lastPos], nullptr, 10));
			if (good)
				lastPos = ++pos;
		} while (good);
		if (v.empty())
			v.push_back(valInt);
	}
	return v;
}

vector<double> ValTree::getFloats(char delim) const
{
	vector<double> v;
	if (!this->isNull())
	{
		size_t pos = 0, lastPos = 0;
		bool good = false;
		do
		{
			pos = val.find(delim, pos);
			good = (pos != string::npos);
			if (good || lastPos > 0)
				v.push_back(strtof(&val[lastPos], nullptr));
			if (good)
				lastPos = ++pos;
		} while (good);
		if (v.empty())
			v.push_back(valFloat);
	}
	return v;
}

void ValTree::addChild(const ValTree& v)
{
	children.push_back(v);
}

void ValTree::removeChild(const string& removeKey)
{
	for (auto it = children.begin(); it != children.end();)
	{
		if (it->key == removeKey)
		{
			it->clear();
			it = children.erase(it);
		}
		else
			++it;
	}
}

bool ValTree::hasChildren() const
{
	return children.size() > 0;
}

int ValTree::size() const
{
	return (int)children.size();
}

//
// regarding the following code duplication, it is safer (for short methods)
// http://stackoverflow.com/questions/856542/elegant-solution-to-duplicate-const-and-non-const-getters
//

ValTree& ValTree::getFirstChild()
{
	return (this->hasChildren() ? children.front() : ValTree::null());
}

const ValTree& ValTree::getFirstChild() const
{
	return (this->hasChildren() ? children.front() : ValTree::null());
}

ValTree& ValTree::getChild(const string& k)
{
	for (auto& v : children)
		if (v.key == k)
			return v;
	return ValTree::null();
}

const ValTree& ValTree::getChild(const string& k) const
{
	for (auto& v : children)
		if (v.key == k)
			return v;
	return ValTree::null();
}

ValTree& ValTree::getIndex(int index)
{
	unsigned i = index;
	return (i < children.size() ? children[i] : ValTree::null());
}

const ValTree& ValTree::getIndex(int index) const
{
	unsigned i = index;
	return (i < children.size() ? children[i] : ValTree::null());
}

ValTree& ValTree::query(const string& query)
{
	auto pos = query.find('.');
	if (pos == string::npos)
		return this->getChild(query);
	
	auto k = query.substr(0, pos);
	auto v = query.substr(pos + 1);
	if (k.size() > 0)
		return this->getChild(k).query(v);
	return this->query(v);
}

const ValTree& ValTree::query(const string& query) const
{
	auto pos = query.find('.');
	if (pos == string::npos)
		return this->getChild(query);
	
	auto k = query.substr(0, pos);
	auto v = query.substr(pos + 1);
	if (k.size() > 0)
		return this->getChild(k).query(v);
	return this->query(v);
}

//
// end duplicate methods
//

void ValTree::addChild(const string& query, const string& _val)
{
	auto pos = query.find('.');
	if (pos == string::npos)
	{
		this->addChild(ValTree(query, _val));
		return;
	}

	auto k = query.substr(0, pos);
	auto subquery = query.substr(pos + 1);
	if (k.size() > 0)
	{
		auto& child = this->getChild(k);
		if (child.isNull())
		{
			ValTree v(k, string());
			v.addChild(subquery, _val);
			this->addChild(v);
		}
		else
		{
			child.addChild(subquery, _val);
		}
	}
}

#pragma mark -
#pragma mark Iteration

ValTree::Iterator::Iterator(const ValTree& v, int i) : index(i), tree(v)
{
}

const ValTree& ValTree::Iterator::operator*()
{
	return tree.getIndex(index);
}

ValTree::Iterator& ValTree::Iterator::operator++()
{
	index++;
	if (index < 0 || index > tree.size())
		index = tree.size();
	return *this;
}

bool ValTree::Iterator::operator!=(const Iterator& rhs)
{
	return index != rhs.index;
}

ValTree::Iterator ValTree::begin() const
{
	return Iterator(*this, 0);
}

ValTree::Iterator ValTree::end() const
{
	return Iterator(*this, this->size());
}

#pragma mark -
#pragma mark Parsing

bool ValTree::parse(const string& data, int& pos, int lastDepth)
{
	int nextLineStart = findAfterNewline(data, pos);
	if (nextLineStart > kMaxFileSize || pos == nextLineStart)
		return false;

	// comment here so jump this line
	if (findNewline(data, pos) <= findNonWhitespace(data, pos))
	{
		pos = nextLineStart;
		return this->parse(data, pos, lastDepth);
	}

	// parse this child
	int depth = getDepth(data, pos);
	if (depth == lastDepth)
	{
		const long dataSize = data.size();

		// key is first word
		int startPos = pos + depth;
		if (startPos < nextLineStart)
		{
			pos = findWhitespace(data, pos + depth + 1);
			key = data.substr(startPos, pos < dataSize ? pos - startPos : string::npos);
		}

		// val is remainder
		if (key.size() > 0)
		{
			pos = findNonWhitespace(data, pos);
			int end = findNewline(data, pos);
			if (pos < dataSize && end > pos)
			{
				val = data.substr(pos, end - pos);
				this->setValInt();
				this->setValFloat();
			}
		}

		pos = nextLineStart;
		depth = getDepth(data, pos);
	}

	// parse children
	if (depth > lastDepth)
	{
		bool success = true;
		lastDepth = depth;
		do
		{
			children.push_back(ValTree());
			success = children.back().parse(data, pos, depth);
			if (!success)
				children.pop_back();
			depth = getDepth(data, pos);
		} while (success && depth == lastDepth);
	}

	return !this->isNull();
}

bool ValTree::parse(const string& filename)
{
	string data;
	ifstream file(filename);
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		auto size = file.tellg();
		data.resize(size, ' ');
		file.seekg(0);
		file.read(&data[0], size);
	}
	if (data.size() <= 0)
		return false;

	return this->parseData(data);
}

bool ValTree::parseData(const string& data)
{
	if (data.size() <= 0)
		return false;

	int pos = 0;
	this->parse(data, pos, -1);
	if (pos > kMaxFileSize)
		cout << "*** WARNING: ValTree parse truncated due to exceeding maximum file size" << endl;
	return true;
}

bool ValTree::save(const string& filename) const
{
	ofstream file(filename);
	if (file.is_open())
	{
		_log(file, *this, -1);
		file.close();
		return true;
	}
	return false;
}

void ValTree::write(string& buffer) const
{
	ostringstream ss;
	_log(ss, *this, -1);
	buffer = ss.str();
}

void ValTree::log() const
{
	stringstream ss;
	_log(ss, *this, -1);
	cout << ss.str() << endl;
}


