
#include "ValTree.h"
#include <iostream>
#include <sys/time.h>

using namespace std;

double time()
{
	const double kMicrosecondFactorInverse = 1.0 / 1000000.0;
	static struct timeval currentTime;
	gettimeofday(&currentTime, nullptr);
	return (currentTime.tv_sec) + (currentTime.tv_usec * kMicrosecondFactorInverse);
}

int main(int argc, const char* argv[])
{
	ValTree v;
	const auto& vconst = v;

	// load / parse the above example
	string filename("Example.txt");
	v.parse(filename);
	cout << filename << " loaded " << (v.isNull() ? "un" : "") << "successfully" << endl;
	
	// retrieve a value
	auto& h = v.getChild("g-is-long").getChild("h");
	cout << "The value of 'g-is-long.h' is '" << h.getStr() << "'" << endl;

	// query the tree
	auto& key41 = v.query("key1.key2.key3.key4-1");
	cout << "The value of 'key1.key2.key3.key4-1' is '" << key41.getStr() << "'" << endl;

	// try a bad query and const version
	auto& key41b = vconst.query("key1.key2..key3.key4-1");
	cout << "The value of 'key1.key2..key3.key4-1' is '" << key41b.getStr() << "'" << endl;

	// store a new value
	v.addChild(ValTree("l", "90,90"));
	cout << "After adding child 'l', new ValTree looks like this:" << endl;
	v.log();

	// store a new value at depth
	v.addTree("l.m.n.o.p", "q");
	cout << "After adding tree 'l.m.n.o.p', new ValTree looks like this:" << endl;
	v.log();
	
	// save the tree to a new file
	filename = "Example-modified.txt";
	v.save(filename);
	cout << "Modified file saved to: " << filename << endl;
	
	// test speed of queries
	{
		cout << "Running query speed test..." << endl;
		const int n = 100 * 1000;
		auto startTime = time();
		for (int i = 0; i < n; ++i)
			v.query("key1.key2.key3.key4-1");
		cout << (int)(n / 1000) << "k queries took " << (int)((time() - startTime) * 1000) << "ms" << endl;
	}

	// test speed of parse
	{
		cout << "Running parse speed test..." << endl;
		const int n = 10 * 1000;
		auto startTime = time();
		for (int i = 0; i < n; ++i)
		{
			ValTree test;
			test.parse(filename);
		}
		cout << (int)(n / 1000) << "k parses took " << (int)((time() - startTime) * 1000) << "ms" << endl;
	}

    return 0;
}


