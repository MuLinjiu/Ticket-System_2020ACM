# Storage

数据操作处理工具文档



## 磁盘读写工具 (Class Storage_IO)

`template<class Value>`: `Value`为存放类型

开头一个头指针`int`表示下一个可用位置的编号

以后每一条记录包含数据`T`和可用位置的编号`int`

如果可用位置编号为`0`表示文本末尾

#### 内部成员

`fstream file`

`int get_pos(int num)`: `return (sizeof(int) + sizeof(Value)) * (num - 1) + sizeof(int);`

`int get_num(int pos)`: `return (pos - sizeof(int)) / (sizeof(int) + sizeof(Value)) + 1;`

#### 构造函数

`Storage_IO()`：默认构造函数

`Storage_IO(string file_name)`：打开`file_name`，如果没有就新建，初始放一个`sizeof(int)`

#### 析构函数

`~Storage_IO()`: 关闭`file_name`文件

#### 接口

* `int write(const Value& val)`: 在文件中找到空位置写入，更新头指针，返回空位置的编号
* `void write(int num, const Value &)`
* 
* `void read(int num, const Value val)`: 找到第`num`条数据读入`val`
* `void del(int num)`: 释放第`num`条数据的空间，头指针指向该空间起始位置



## B+ Tree (Class BpTree)

`template<class Key, class Value>`: `Key`为键值的类型，`Value`为存储类型

#### 存储方式

* 树节点结构
  * 键值个数
  * 是否为叶节点
  * （若是叶节点）前驱、后继叶节点
  * 键值数组
  * 子树编号数组（若为叶节点，存放数据索引）
*  最大子节点个数、根节点编号
*  B+树基本信息文件
* 磁盘读写工具对象
  *  保存B+树节点的文件
  *  保存索引对应的`Value`类型数据的文件

#### 构造函数

`BpTree(string file_name)`

文件中如果找到`file_name`，就初始化根节点编号

如果找不到`file_name`，就新建，初始时根为叶节点

#### 内部实现

* 节点分裂`int split(int child, node &y)`:  将编号为`child`的节点`y`分裂，返回分裂后新增节点的编号

* 插入数据`void insert(int current, const Key &key, int index)`: 递归找到对应叶节点，返回时判断孩子节点是否需要分裂
* 删除数据`void delete(int current, const Key &key)`: 递归找到对应叶节点，返回时判断孩子是否不满足要求。如果不满足要求，如果不是第一个键值，就往前找兄弟，否则往后找兄弟。如果兄弟借不到键值，就合并。
* 查找数据`node search(int current, const Key &key)`: 以键值搜索返回对应的叶节点内容，如果查找失败返回`0`

#### 接口
* 插入数据`void insert(const Key &key, const Value &val)`: 在数据文件中添加一条数据，把索引按照`key`插入树中（注意调用函数之后判断根节点是否要分裂）
* 删除数据`void delete(const Key &key)`: 在树中按照`key`找到索引，在数据文件中删除
* 查找数据`vector<Value> search(const Key &key, bool (*equ)(Key, Key))`:  在树中找到`key`相同的所有数据，相等的含义用函数指针`*equ`描述



## 文件命名

* 保存B+树节点信息的文件：`bpt_`+`file_name`+`_node`+`.dat`
* 保存B+树总体基本信息的文件： `bpt_`+`file_name`+`_basic`+`.dat`
* 保存`Value`类型原始数据的文件： `data_`+`file_name`+`.dat`

