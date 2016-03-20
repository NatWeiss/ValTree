///
/// ValTree
///

#pragma once

#include <vector>
#include <string>

class ValTree
{
	public:
		/// Blank constructor is necessary for storing ValTree objects in a vector.
		ValTree();
	
		/// Construct a ValTree node using a key / value pair.
		ValTree(const std::string& key, const std::string& val);
	
		/// Copy constructor.
		ValTree(const ValTree& rhs);
	
		/// Destructor.
		~ValTree();
	
		/// Completely clear the ValTree of key, value, children and siblings.
		void clear();
	
		/// Return true if the key and value are empty.
		bool isNull() const;

		/// Getters.
		const std::string& getKey() const;
		const std::string& getStr() const;
		long getInt() const;
		double getFloat() const;
	
		/// Set the key and value.
		void set(const std::string& key, const std::string& val);
	
		/// Assignment operators (these only work if this ValTree object already has a key).
		ValTree& operator=(const ValTree& rhs);
		ValTree& operator=(const std::string& rhs);
		ValTree& operator=(int rhs);
		ValTree& operator=(double rhs);

		/// Comparison operators.
		bool operator==(const ValTree& rhs);
		bool operator!=(const ValTree& rhs);
		bool operator<(const ValTree& rhs);
		bool operator>(const ValTree& rhs);
	
		/// Return the size of this ValTree (0 if null, 1 + number of children otherwise).
		int size() const;
	
		/// Return a sibling given index (0 is this ValTree, 1 is the first child).
		ValTree& getIndex(int index);
		const ValTree& getIndex(int index) const;

		/// Return true if the tree has children.
		bool hasChildren() const;
	
		/// Return the first child object (returns a static null ValTree if no children).
		ValTree& getFirstChild();
		const ValTree& getFirstChild() const;
	
		/// Return a child object given a key (returns a static null ValTree if no children).
		ValTree& getChild(const std::string& key);
		const ValTree& getChild(const std::string& key) const;

		/// Return a child object using a dot-separated query string to navigate the tree.
		ValTree& query(const std::string& query);
		const ValTree& query(const std::string& query) const;

		/// Add a child to the current ValTree.
		void addChild(const ValTree& v);

		/// Add child value and all necessary branches.
		void addChild(const std::string& query, const std::string& val);
	
		/// Parse the given file into this ValTree object.
		bool parse(const std::string& filename);

		/// Parse the given data string into this ValTree object.
		bool parseData(const std::string& data);
	
		/// Save this ValTree object into the given file.
		bool save(const std::string& filename);
	
		/// Log out this ValTree to cout / stdout.
		void log();

		/// Used to iterate over the ValTree's siblings, for example: `for (auto& sibling : tree) {}`
		class Iterator
		{
			private:
				int index;
				const ValTree& tree;
			
			public:
				Iterator(const ValTree& tree, int index);
				const ValTree& operator*();
				Iterator& operator++(); // note this is pre-increment only (ie. ++it)
				bool operator!=(const Iterator& rhs);
		};
	
		/// Iteration.
		Iterator begin() const;
		Iterator end() const;

	private:
		std::string key;
		std::string val;
		long valInt;
		double valFloat;
		std::vector<ValTree> children;

		void setValInt();
		void setValFloat();
		static ValTree& null();
	
		bool parse(const std::string& data, int& pos, int lastDepth);
};


