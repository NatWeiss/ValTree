
#include "ValTree.h"
#include <iostream>

using namespace std;

int main(int argc, const char* argv[])
{
	// load / parse the above example
	string filename("Example.txt");
	ValTree v;
	v.parse(filename);
	cout << filename << " loaded " << (v.isNull() ? "un" : "") << "successfully" << endl;
	
	// retrieve a value
	auto& h = v.getChild("g-is-long").getChild("h");
	cout << "The value of 'g-is-long.h' is '" << h.getStr() << "'" << endl;
	
	// store a new value
	v.addChild(ValTree("l", "90,90"));
	cout << "After adding sibling 'l', new ValTree looks like this:" << endl;
	v.log();
	
	// save the tree to a new file
	filename = "Example-modified.txt";
	v.save(filename);
	cout << "Modified file saved to: " << filename << endl;

    return 0;
}
