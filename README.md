ValTree
-------

ValTree is a file format for storing key value pairs in a hierarchy -- more efficient than JSON -- and a C++ class to read / write these files.

There are two problems with JSON:

1. It can be annoying to have to format data using braces, colons and quotes. (You cannot be lazy.)
2. The order of data is not guaranteed.

ValTree solves these two issues by making it more simple to type / create files and guarantees the order of values.


Ports to Other Languages
------------------------

* [C# port](https://github.com/wackoisgod/ValTreeCSharp) -- thanks to [wackoisgod](https://github.com/wackoisgod)


Example File
------------

ValTree stores files with any file extension, though `.txt` is encouraged. Here is an example file, `Example.txt`:

	a
		b
			c
			d  1
			e  2.01
			f  something
	g-is-long
		h  h is a cool letter
		i
	j  1.618
	k-is-longer  too

Tabs or spaces can be used to indent the data. Each tab or space puts the current line of data as a child of the previous line, depending on depth.

Data is stored as key value pairs. After the initial tabs or spaces comes the key, then as many tabs or spaces as desired, then the value. The key can be any non-whitespace character. The value is everything remaining on the current line after the key and some whitespace. (Note that values can contain whitespace.)

Values can be strings, integers or floats. When parsing, `ValTree` reads the value as a string, then also converts it to `long` and `double` using `strtol` and `strtod`. All three types of data are stored simultaneously as separate data members for quick access.

Each `ValTree` is a self-contained recursive class containing siblings and children. Children are `ValTree` objects stored at one level deeper than current. Siblings are `ValTree` objects stored at the same depth level. Parsing a `ValTree` puts all siblings inside the first `ValTree` object for convenience.


Example Code
------------

Here is some example code to load a ValTree, retrieve a value, store a new value, and save:

	// load / parse the above example
	ValTree v;
	v.parse("Example.txt");
	
	// retrieve a value
	auto& h = v.getChild("g-is-long").getChild("h");
	cout << "The value of 'h' is " << h.getStr() << endl;
	
	// query the tree
	auto& key41 = v.query("key1.key2.key3.key4-1");
	cout << "The value of 'key1.key2.key3.key4-1' is '" << key41.getStr() << "'" << endl;

	// store a new value
	v.addChild(ValTree("l", "90,90"));
	
	// save the tree to a new file
	v.save("Example-modified.txt");

To Do
-----

1. Add the ability to get siblings as a vector.

License
-------

ValTree is licensed under the MIT license.


