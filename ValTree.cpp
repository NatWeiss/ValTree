//
// ValTree
//

#include "ValTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

const int   kMaxFileSize      = 8 * 1024 * 1024;
const char* kCommentStartTag  = "//";

using namespace std;

//
// static helper functions
//

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

static int findCommentOrNewline(const string& s, int start)
{
  int i = start;
  int nl = findNewline(s, start);
  int commentIndex = 0;
  static const int commentTagSize = (int)strlen(kCommentStartTag);
  
  while (i < nl) {
    if (s[i++] == kCommentStartTag[commentIndex++])
    {
      if (commentIndex == commentTagSize)
        return i - commentTagSize;
    }
    else
      commentIndex = 0;
  }
  
  return i;
}


static void _log(ostream& ss, const ValTree& v, int depth)
{
	if (!v.isNull())
	{
		for (int i = 0; i < depth; i++)
			ss << "\t";
		ss << v.getKey() << "  " << v.getStr() << endl;
	}
  
  for (int i = 0; i < v.size(); i++) {
    auto child = v.getIndex(i);
    _log(ss, child, depth + 1);
  }
	
	// note that a c++98 for loop is not any faster for saving large files
	//for (auto it = v.begin(), end = v.end(); it != end; ++it)
}

// Split a string using a char as delimiter
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

std::string join(const std::vector<std::string> pieces, char delim) {
  stringstream ss;
  if (pieces.size()) {
    ss << pieces[0];
    for (int i = 1; i < pieces.size(); i++) {
      ss << delim << pieces[i];
    }
  }
  return ss.str();
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
	val.clear();
	valInt = 0;
	valFloat = 0.0;
	children.clear();
}

bool ValTree::isNull() const
{
	return (key.size() <= 0 && val.size() <= 0 && children.size() == 0);
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

void ValTree::addChild(const ValTree& v)
{
	children.push_back(v);
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

int ValTree::size() const
{
  return (int)children.size();
}

ValTree& ValTree::getIndex(int index)
{
	if (index >= 0 && index < children.size())
    return children[index];
  return null();
}

const ValTree& ValTree::getIndex(int index) const
{
  if (index >= 0 && index < children.size())
    return children[index];
  return null();
}

ValTree& ValTree::query(const std::string &query) {
  auto keys = split(query, '.');
  if (keys.size()) {
    string key = keys[0];
    if (keys.size() > 1) {
      keys.erase(std::find(keys.begin(), keys.end(), key));
      return getChild(key).query(join(keys, '.'));
    } else {
      return getChild(key);
    }
  }
  return null();
}

const ValTree& ValTree::query(const std::string &query) const {
  auto keys = split(query, '.');
  if (keys.size()) {
    string key = keys[0];
    if (keys.size() > 1) {
      keys.erase(std::find(keys.begin(), keys.end(), key));
      return getChild(key).query(join(keys, '.'));
    } else {
      return getChild(key);
    }
  }
  return null();
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

int ValTree::getDepth(const string& data, int pos)
{
	for (int i = pos; i < data.size(); i++)
		if (!isWhitespace(data[i]))
			return i - pos;
	return -1;
}

bool ValTree::parse(const string& data, int& pos, int currentDepth)
{
  int lineEnd = findNewline(data, pos);
  int nextLineStart = findAfterNewline(data, pos);
  
  if (nextLineStart > kMaxFileSize)
    return false;
  
  // pait<int, ValTree> collects a ValTree with its current depth
  vector<pair<int,ValTree*>> tree;
  
  // while we still have lines ...
  do {
    // Verify if is a full-line comment
    int commentStart = findCommentOrNewline(data, pos);
    int firstCharPos = findNonWhitespace(data, pos);
    
    if (commentStart <= firstCharPos) {
      // Comment here so jump this line
      pos = nextLineStart;
    }
    int depth = this->getDepth(data, pos);
    
    // key is first word
    int startPos = pos + depth;
    
    string _key;
    
    if (startPos < nextLineStart)
    {
      pos = findWhitespace(data, pos + depth + 1);
      _key = data.substr(startPos, pos < data.size() ? pos - startPos : string::npos);
    }
    
    // val is remainder
    if (_key.size() > 0)
    {
      ValTree* newObj = new ValTree;
      newObj->key = _key;
      
      pos = findNonWhitespace(data, pos);
      // Trim the remainder if we find a comment
      int end = findCommentOrNewline(data, pos);
      
      if (pos < data.size() && end > pos)
      {
        newObj->val = data.substr(pos, end - pos);
        newObj->setValInt();
        newObj->setValFloat();
      }
      
      tree.push_back({depth, newObj});
    }
    
    pos = nextLineStart;
    lineEnd = findNewline(data, pos);
    nextLineStart = findAfterNewline(data, pos);
  } while (pos != lineEnd && lineEnd < kMaxFileSize);
  
  // Associate each read line to create the actual tree
  std::stack<pair<int, ValTree*>> cursor;
  cursor.push({-1,this});
  
  for (auto _pair : tree) {
    auto valDepth = _pair.first;
    auto val = _pair.second;
    
    if (valDepth < 0)
      continue;
    
    do {
      auto temp = cursor.top();
      auto depth = temp.first;
      auto parent = temp.second;
      
      if (valDepth > depth) {
        cursor.push({valDepth, val});
        break;
      } else if (valDepth == depth) {
        cursor.pop();
        cursor.top().second->children.push_back(*parent);
        cursor.push({valDepth, val});
        break;
      } else {
        cursor.pop();
        cursor.top().second->children.push_back(*parent);
      }
      
    } while (cursor.size());
  }
  
  // Append remaining objects to the main tree
  while (cursor.size() > 1) {
    auto top = cursor.top();
    cursor.pop();
    cursor.top().second->children.push_back(*top.second);
  }
  
  return true;
}

bool ValTree::parse(const string& filename)
{
	this->clear();

	string data;
	ifstream file(filename);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
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
	this->clear();

	if (data.size() <= 0)
		return false;

	int pos = 0;
	this->parse(data, pos, true);
	if (pos > kMaxFileSize)
		cout << "*** WARNING: ValTree parse truncated due to exceeding maximum file size" << endl;
	return true;
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
	_log(ss, *this, -1);
	cout << ss.str() << endl;
}


