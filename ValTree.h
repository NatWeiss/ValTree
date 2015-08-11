///
/// ValTree
///

#pragma once

#include <vector>
#include <string>

class ValTree
{
	private:
		std::string key;
		std::string val;
		long valInt;
		double valFloat;
		std::vector<ValTree> children;
		std::vector<ValTree> siblings;
	
		void setValInt();
		void setValFloat();
		static ValTree& null();
	
		int getDepth(const std::string& data, int pos);
		bool parse(const std::string& data, int& pos, bool firstSibling);
		
	public:
		class Iterator
		{
			private:
				int index;
				const ValTree& tree;
			
			public:
				Iterator(const ValTree& tree, int index);
				const ValTree& operator*();
				Iterator& operator++();
				bool operator!=(const Iterator& rhs);
		};
	
		ValTree();
		ValTree(const std::string& key, const std::string& val);
		ValTree(const ValTree& rhs);
		~ValTree();
	
		void clear();
		bool isNull() const;
	
		void set(const std::string& key, const std::string& val);
		ValTree& operator=(const ValTree& rhs);
		ValTree& operator=(const std::string& rhs);
		ValTree& operator=(int rhs);
		ValTree& operator=(double rhs);

		bool operator==(const ValTree& rhs);
		bool operator!=(const ValTree& rhs);
		bool operator<(const ValTree& rhs);
		bool operator>(const ValTree& rhs);
	
		const std::string& getKey() const;
		const std::string& getStr() const;
		int getInt() const;
		double getFloat() const;
	
		void addChild(const ValTree& v);
		void addSibling(const ValTree& v);

		bool hasChildren() const;
		ValTree& getFirstChild();
		const ValTree& getFirstChild() const;
		ValTree& getChild(const std::string& key);
		const ValTree& getChild(const std::string& key) const;
		ValTree& getSibling(const std::string& key);
		const ValTree& getSibling(const std::string& key) const;
	
		int size() const;
		ValTree& getIndex(int index);
		const ValTree& getIndex(int index) const;
		Iterator begin() const;
		Iterator end() const;
	
		bool parse(const std::string& filename);
		bool save(const std::string& filename);
		void log();
};


