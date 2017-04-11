# 实验一：近似查询 实验报告

### 计45 侯禺凡 2014011433

## 一、实验要求

参考框架代码，实现SimSearcher类的以下方法：

- createIndex()函数
- searchJaccard()函数
- searchED()函数

从而实现对Jaccard和ED两种方式的近似查询。

## 二、实验平台

- 操作系统：Ubuntu 14.04 LTS
- 编译器：gcc version 4.8.4

## 三、设计思路

首先，采用STL里的集合数据结构`std::set`来存储每一行的字符串，便于之后使用索引来获取：

	vector<Entry> word_vec;
	//vector of lines in input file

其次，需要建立倒排列表inverted list。为了更快地进行查找，我采用STL里的`std::set`来存储倒排列表里的项：

	set<Qgram> qgram_set;
	//set of gram for ED
	set<Qgram> word_set;
	//set of word for JAC

其中一个是为了ED方式而构建的，另一个是为了Jaccard方式构建的。而Qgram类型定义如下：

	struct Qgram
	//a gram(for ED) or a word(for JAC)
	{
		char* qgram;
		int index;//inverted list index
	};

其中`char* qgram`变量是ED方法里的gram或者Jaccard方法里被空格分开的单词，而`int index`是倒排列表二维数组第一维的索引，可以理解为一个“指针”指向倒排列表里对应一行。

为了存储倒排列表里的内容，我声明了一个STL里`std::vector`类型变量：

	vector<vector<int> > inverted_list;
	//inverted list for ED
	vector<vector<int> > word_inverted_list;
	//inverted list for JAC

这实际上是一个二维数组，其中第一维表示前述set里的gram或者word，第二维表示含有该gram或者word的全部字符串，是前述word_vec变量的索引。Qgram类的index成员变量是这个变量第一维的索引。

这样建立倒排表之后，对于一个查询，首先按ED方法或者Jaccard方法的要求将其分割成gram或者word。然后，对每一个gram或者word，在set里查找出一个Qgram类型变量。然后用里面的index成员变量作为索引即可获取这一gram或word在倒排列表里列表项。根据前述对两个`vector<vector<int> >`变量的说明，列表项里的每一项都是`word_vec`向量的索引，从而获取到对应的字符串。

## 四、实现难点

实现难点主要有以下几点：

- 对倒排列表的数据结构的设计。若是直接使用一个向量来存储倒排列表里的项，插入非常方便，但是在查找时最坏将必须遍历整个倒排列表，因此使用了set数据结构来存储，查找gram或word的开销降低为O(logn)。
- STL的使用和调试。由于用到了set和vector等数据结构，我使用STL来简化代码，但也因此遇到了不少编译不通过的情况，在这上面耗费了大量时间。
- 单次评测时间长。由于数据量巨大，算法复杂度也不低，导致单次评测时间较长，给实验的完成增加了一定的困难。

## 五、实验结果

我的代码已经通过[**Online Judge**](http://166.111.71.175:8007/dashboard/)上的exp1和exp1-final测试集，最好结果分别如下：

	|test set|Memory Usage(GB)|Time Usage(s)|
	|--------|----------------|-------------|
	|exp1|0.673|33.173|
	|exp1-final|1.826|251.228|

在平台上**标记了exp1-final的某个版本**，结果如下（ID：2848）：

	|test set|Memory Usage(GB)|Time Usage(s)|
	|--------|----------------|-------------|
	|exp1-final|1.826|254.252|

