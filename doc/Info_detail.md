# Infomation Detail 
`信息存储具体方法`
## 自定义类
### 用户类
一个用户具有如下数据：username， password， name，mailAddr， privilege
其中username对应一棵B+树，可进行查询与修改操作。
用户登录操作用自己写的map类实现。

### 车次类
一个车次内部信息有：trainID,stationNum,station(char*数组)，seatNum，prices(int 数组),startetime,traveltime(数组)，stopoverTimes(数组)，saleDate，Type。

###车站类
成员为车站名称并 记录经过该站的车次信息。

###日期类
三个数字：年，月，日。自定义加减法。


