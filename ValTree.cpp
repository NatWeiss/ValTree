//
// ValTree
//

#include "ValTree.h"
#include <fstream>

using namespace std;

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
	val.clear();
	valInt = 0;
	valFloat = 0.0;
	children.clear();
	siblings.clear();
}

bool ValTree::isNull() const
{
	return (key.size() <= 0 && val.size() <= 0);
}

void ValTree::setValInt()
{
	valInt = strtol(val.c_str(), nullptr, 10);
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

ValTree& ValTree::operator=(const ValTree& rhs)
{
	key = rhs.key;
	val = rhs.val;
	valInt = rhs.valInt;
	valFloat = rhs.valFloat;
	children = rhs.children;
	siblings = rhs.siblings;
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

const string& ValTree::getKey() const
{
	return key;
}

const string& ValTree::getStr() const
{
	return val;
}

int ValTree::getInt() const
{
	return valInt;
}

double ValTree::getFloat() const
{
	return valFloat;
}

void ValTree::addChild(const ValTree& v)
{
	children.push_back(v);
}

void ValTree::addSibling(const ValTree& v)
{
	siblings.push_back(v);
}

bool ValTree::hasChildren() const
{
	return children.size() > 0;
}

// regarding this code duplication, it is safer (for short methods)
// http://stackoverflow.com/questions/856542/elegant-solution-to-duplicate-const-and-non-const-getters

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

ValTree& ValTree::getSibling(const string& k)
{
	if (key == k)
		return *this;
	for (auto& v : siblings)
		if (v.key == k)
			return v;
	return ValTree::null();
}

const ValTree& ValTree::getSibling(const string& k) const
{
	if (key == k)
		return *this;
	for (auto& v : siblings)
		if (v.key == k)
			return v;
	return ValTree::null();
}

int ValTree::size() const
{
	return (this->isNull() ? 0 : 1) + siblings.size();
}

ValTree& ValTree::getIndex(int index)
{
	if (index > 0 && index - 1 < siblings.size())
		return siblings[index - 1];
	return *this;
}

const ValTree& ValTree::getIndex(int index) const
{
	if (index > 0 && index - 1 < siblings.size())
		return siblings[index - 1];
	return *this;
}

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

static bool isWhitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static int findWhitespace(const string& s, int start)
{
	int i = start;
	for (; i < s.size() && !isWhitespace(s[i]); i++)
		;
	return i;
}

static int findNonWhitespace(const string& s, int start)
{
	int i = start;
	for (; i < s.size() && isWhitespace(s[i]) && s[i] != '\n' && s[i] != '\r'; i++)
		;
	return i;
}

static int findNewline(const string& s, int start)
{
	int i = start;
	for (; i < s.size() && s[i] != '\n' && s[i] != '\r'; i++)
		;
	return i;
}

static int findAfterNewline(const string& s, int start)
{
	int i = findNewline(s, start);
	for (; i < s.size() && (s[i] == '\n' || s[i] == '\r'); i++)
		;
	return i;
}

int ValTree::getDepth(const string& data, int pos)
{
	for (int i = pos; i < data.size(); i++)
		if (!isWhitespace(data[i]))
			return i - pos;
	return -1;
}

bool ValTree::parse(const string& data, int& pos, bool firstSibling)
{
	// get depth
	int depth = this->getDepth(data, pos);
	if (depth < 0)
		return false;
	int nextPos = findAfterNewline(data, pos);
	int childDepth = this->getDepth(data, nextPos);
	
	// key is first word
	int startPos = pos + depth;
	pos = findWhitespace(data, pos + depth + 1);
	key = data.substr(startPos, pos < data.size() ? pos - startPos : string::npos);
	
	// val is remainder
	if (key.size() > 0)
	{
		pos = findNonWhitespace(data, pos);
		int end = findNewline(data, pos);
		if (pos < data.size() && end > pos)
		{
			val = data.substr(pos, end - pos);
			this->setValInt();
			this->setValFloat();
		}
	}

	// cue up next line
	pos = nextPos;

	// parse children
	if (childDepth > depth)
	{
		ValTree v;
		if (v.parse(data, pos, true))
			children.push_back(v);
		childDepth = this->getDepth(data, pos);
	}
	
	// siblings
	if (childDepth == depth && firstSibling)
	{
		bool success = true;
		while (success && childDepth == depth)
		{
			ValTree v;
			success = v.parse(data, pos, false);
			if (success)
				siblings.push_back(v);
			childDepth = this->getDepth(data, pos);
		}
		return siblings.size() > 0;
	}
	return !this->isNull();
}

bool ValTree::parse(const string& filename)
{
	this->clear();

	auto data = FileUtils::getInstance()->getStringFromFile(filename);
	if (data.size() <= 0)
		return false;

	int pos = 0;
	this->parse(data, pos, true);
	return true;
}

static void _log(ostream& ss, const ValTree& v, int depth)
{
	for (int i = 0; i < depth; i++)
		ss << "\t";
	ss << v.getKey() << "  " << v.getStr() << endl;
	
	if (v.hasChildren())
		for (auto& child : v.getFirstChild())
			_log(ss, child, depth + 1);
	int i = 0;
	for (auto& sibling : v)
		if (i++ > 0)
			_log(ss, sibling, depth);
}

bool ValTree::save(const string& filename)
{
	ofstream file(filename);
	if (file.is_open())
	{
		_log(file, *this, 0);
		file.close();
		return true;
	}
	return false;
}

void ValTree::log()
{
	stringstream ss;
	_log(ss, *this, 0);
	Log("%s", ss.str().c_str());
}


